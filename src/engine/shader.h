#pragma once

class shader
{
public:
    virtual ~shader() = default;

    virtual void use() const = 0;
    virtual void reload()    = 0;

    virtual void set_uniform1(const char* name, int value)      = 0;
    virtual void set_uniform1(const char* name, uint32_t value) = 0;
    virtual void set_uniform1(const char* name, float value)    = 0;

    // void set_uniform1v(const char* name, int* value, uint32_t count);
    // void set_uniform1v(const char* name, uint* value, uint32_t count);
    // void set_uniform1v(const char* name, float* value, uint32_t count);

    virtual void set_uniform2(const char* name, int val1, int val2)     = 0;
    virtual void set_uniform2(const char* name,
                              uint32_t    val1,
                              uint32_t    val2)                            = 0;
    virtual void set_uniform2(const char* name, float val1, float val2) = 0;

    // void set_uniform2v(const char* name, int* val1, int* val2, uint32_t
    // count); void set_uniform2v(const char* name, uint* val1, uint* val2,
    // uint32_t count); void set_uniform2v(const char* name, float* val1, float*
    // val2, uint count);

    virtual void set_uniform3(const char* name,
                              int         val1,
                              int         val2,
                              int         val3)      = 0;
    virtual void set_uniform3(const char* name,
                              uint32_t    val1,
                              uint32_t    val2,
                              uint32_t    val3) = 0;
    virtual void set_uniform3(const char* name,
                              float       val1,
                              float       val2,
                              float       val3)    = 0;

    // void set_uniform3v(
    //     const char* name, int* val1, int* val2, int* val3, uint32_t count);
    // void set_uniform3v(
    //     const char* name, uint* val1, uint* val2, uint* val3, uint32_t
    //     count);
    // void set_uniform3v(
    //     const char* name, float* val1, float* val2, float* val3, uint32_t
    //     count);

    virtual void set_uniform4(
        const char* name, int val1, int val2, int val3, int val4) = 0;
    virtual void set_uniform4(const char* name,
                              uint32_t    val1,
                              uint32_t    val2,
                              uint32_t    val3,
                              uint32_t    val4)                      = 0;
    virtual void set_uniform4(
        const char* name, float val1, float val2, float val3, float val4) = 0;

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

protected:
    virtual void load(const char* path, int type) = 0;
};