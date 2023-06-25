#pragma once
#include "core/types.h"
#include "shader.h"

class shader_opengl : public shader
{
public:
    shader_opengl(const char* path_to_vertex, const char* path_to_fragment);
    ~shader_opengl();

    uint32_t get_program_id() const;

    void use() const override;
    void reload() override;

    void set_uniform1(const char* name, int value) override;
    void set_uniform1(const char* name, uint32_t value) override;
    void set_uniform1(const char* name, float value) override;

    // void set_uniform1v(const char* name, int* value, uint32_t count);
    // void set_uniform1v(const char* name, uint* value, uint32_t count);
    // void set_uniform1v(const char* name, float* value, uint32_t count);

    void set_uniform2(const char* name, int val1, int val2) override;
    void set_uniform2(const char* name, uint32_t val1, uint32_t val2) override;
    void set_uniform2(const char* name, float val1, float val2) override;

    // void set_uniform2v(const char* name, int* val1, int* val2, uint32_t
    // count); void set_uniform2v(const char* name, uint* val1, uint* val2,
    // uint32_t count); void set_uniform2v(const char* name, float* val1, float*
    // val2, uint count);

    void set_uniform3(const char* name, int val1, int val2, int val3) override;
    void set_uniform3(const char* name,
                      uint32_t    val1,
                      uint32_t    val2,
                      uint32_t    val3) override;
    void set_uniform3(const char* name,
                      float       val1,
                      float       val2,
                      float       val3) override;

    // void set_uniform3v(
    //     const char* name, int* val1, int* val2, int* val3, uint32_t count);
    // void set_uniform3v(
    //     const char* name, uint* val1, uint* val2, uint* val3, uint32_t
    //     count);
    // void set_uniform3v(
    //     const char* name, float* val1, float* val2, float* val3, uint32_t
    //     count);

    void set_uniform4(
        const char* name, int val1, int val2, int val3, int val4) override;
    void set_uniform4(const char* name,
                      uint32_t    val1,
                      uint32_t    val2,
                      uint32_t    val3,
                      uint32_t    val4) override;
    void set_uniform4(const char* name,
                      float       val1,
                      float       val2,
                      float       val3,
                      float       val4) override;

    // void set_uniform4v(const char* name,
    //                    int*        val1,
    //                    int*        val2,
    //                    int*        val3,
    //                    int*        val4,
    //                    uint32_t        count);
    // void set_uniform4v(const char* name,
    //                    uint*       val1,
    //                    uint*       val2,
    //                    uint*       val3,
    //                    uint*       val4,
    //                    uint32_t        count);
    // void set_uniform4v(const char* name,
    //                    float*      val1,
    //                    float*      val2,
    //                    float*      val3,
    //                    float*      val4,
    //                    uint32_t        count);

private:
    void load(const char* path, int type) override;

    const char* path_to_vertex;
    const char* path_to_fragment;

    uint32_t program;
};