extern "C"
{
#include "load_bin.h"
}
#include <sleigh/libsleigh.hh>
#include <cassert>

namespace
{

class InMemoryLoadImage : public LoadImage
{
public:
    explicit InMemoryLoadImage(std::string&& image_buffer)
        : LoadImage("nofile"), image_buffer(std::move(image_buffer)) {}

    void loadFill(unsigned char *ptr, int size, const Address &addr) override
    {
        uint64_t start = addr.getOffset();
        assert(start + size <= image_buffer.size());
        memcpy(ptr, &image_buffer[start], size);
    }

    std::string getArchType(void) const override
    {
        return "memory";
    }

    void adjustVma(long) override {}

private:
    std::string image_buffer;
};

class AssemblyNoop : public AssemblyEmit
{
public:
    void dump(const Address &, const std::string &,
              const std::string &) override {}
};

} // namespace

int main(int argc, char *argv[])
{
    uint8_t *code = nullptr;
    size_t code_len = 0, loop_count = 0;
    if (!read_file(argc, argv, &code, &code_len, &loop_count))
    {
        return 1;
    }
    std::string image_buffer(reinterpret_cast<char *>(code), code_len);

    // Initialize SLEIGH
    InMemoryLoadImage load_image(std::move(image_buffer));
    ContextInternal ctx;
    Sleigh engine(&load_image, &ctx);
    DocumentStorage storage;
    const auto sla_path = sleigh::FindSpecFile("x86-64.sla");
    if (!sla_path)
    {
        fputs("Couldn't find sla file\n", stderr);
        free(code);
        return 1;
    }
    Element *sla_doc = storage.openDocument(*sla_path)->getRoot();
    storage.registerTag(sla_doc);
    Element *pspec_doc = storage.openDocument(
        "bench/sleigh/_deps/ghidrasource-src/Ghidra/Processors/x86/data/languages/x86-64.pspec"
        )->getRoot();
    storage.registerTag(pspec_doc);
    engine.initialize(storage);
    engine.allowContextSet(false);
    // Now that context symbol names are loaded by the translator
    // we can set the default context
    // This imitates what is done in
    //   void Architecture::parseProcessorConfig(DocumentStorage &store)
    const Element *el = storage.getTag("processor_spec");
    if (el)
    {
        const List &list(el->getChildren());
        for (const auto *iter : list)
        {
            const string &elname(iter->getName());
            if (elname == "context_data")
            {
                ctx.restoreFromSpec(iter, &engine);
                break;
            }
        }
    }

    AssemblyNoop asm_emit;
    size_t num_valid_insns = 0, num_bad_insn = 0;
    clock_t start_time = clock();
    Address end_addr(engine.getDefaultCodeSpace(), code_len);
    for (size_t round = 0; round < loop_count; ++round)
    {
        Address cur_addr(engine.getDefaultCodeSpace(), 0);
        while (cur_addr < end_addr)
        {
            try
            {
                int32_t instr_len = engine.printAssembly(asm_emit, cur_addr);
                cur_addr = cur_addr + instr_len;
                ++num_valid_insns;
            }
            catch (BadDataError&)
            {
                cur_addr = cur_addr + 1;
                ++num_bad_insn;
            }
        }
    }
    clock_t end_time = clock();

    printf(
        "Disassembled %zu instructions (%zu valid, %zu bad), %.2f ms\n",
        num_valid_insns + num_bad_insn,
        num_valid_insns,
        num_bad_insn,
        (double)(end_time - start_time) * 1000.0 / CLOCKS_PER_SEC
    );

    free(code);
    return 0;
}
