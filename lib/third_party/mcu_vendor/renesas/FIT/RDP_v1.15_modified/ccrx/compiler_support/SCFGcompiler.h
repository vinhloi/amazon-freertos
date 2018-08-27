#ifndef SCFGCOMPILERS_H
#define SCFGCOMPILERS_H

/*
https://gcc-renesas.com/migration-guides/rx/index.html#Compiler_directives

https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html
https://gcc.gnu.org/onlinedocs/gcc/RX-Function-Attributes.html

https://gcc.gnu.org/onlinedocs/gcc/Variable-Attributes.html
https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html
*/

#if defined(__CCRX__)
    /* supported */
#elif defined(__GNUC__)
    /* supported */
#elif defined(__ICCRX__)
    /* in preparation */
    /* http://ftp.iar.se/WWWfiles/RX/webic/doc/EWRX_MigratingFromRenesas.pdf */
    /* http://ftp.iar.se/WWWfiles/RX/webic/doc/EWRX_DevelopmentGuide.ENU.pdf */
#else
    #error "Unrecognized compiler"
#endif

#define R_PRAGMA(...)    _Pragma(#__VA_ARGS__)

#if defined(__CCRX__)

#define R_PRAGMA_PACK          R_PRAGMA(pack)
#define R_PRAGMA_UNPACK        R_PRAGMA(unpack)
#define R_PRAGMA_PACKOPTION    R_PRAGMA(packoption)

#elif defined(__GNUC__)

#define R_PRAGMA_PACK          R_PRAGMA(pack(1))
#define R_PRAGMA_UNPACK        R_PRAGMA(pack(4))
#define R_PRAGMA_PACKOPTION    R_PRAGMA(pack())

#elif defined(__ICCRX__)

#define R_PRAGMA_PACK          R_PRAGMA(pack(1))
#define R_PRAGMA_UNPACK        R_PRAGMA(pack(4))
#define R_PRAGMA_PACKOPTION    R_PRAGMA(pack())

#endif

#if defined(__CCRX__)

#define R_PRAGMA_INTERRUPT(function_name, vector)            R_PRAGMA(interrupt function_name(vect=vector))\
                                                             extern void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT(function_name, vector)     R_PRAGMA(interrupt function_name(vect=vector))\
                                                             static void function_name(void);

#define R_PRAGMA_INTERRUPT_FUNCTION(function_name)           R_PRAGMA(interrupt function_name)\
                                                             extern void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT_FUNCTION(function_name)    R_PRAGMA(interrupt function_name)\
                                                             static void function_name(void);

#define R_PRAGMA_FAST_INTERRUPT(function_name)               R_PRAGMA(interrupt function_name(fast))\
                                                             extern void function_name(void);
#define R_PRAGMA_STATIC_FAST_INTERRUPT(function_name)        R_PRAGMA(interrupt function_name(fast))\
                                                             static void function_name(void);

#define R_ATTRIB_INTERRUPT                                   extern /* only this one because of no corresponding keyword */
#define R_ATTRIB_STATIC_INTERRUPT                            static /* only this one because of no corresponding keyword */

#define R_ATTRIB_FAST_INTERRUPT                              extern /* only this one because of no corresponding keyword */
#define R_ATTRIB_STATIC_FAST_INTERRUPT                       static /* only this one because of no corresponding keyword */

#elif defined(__GNUC__)

#define R_PRAGMA_INTERRUPT(function_name, vector)            extern void function_name(void) __attribute__((interrupt(".rvectors", vector)));
#define R_PRAGMA_STATIC_INTERRUPT(function_name, vector)     static void function_name(void) __attribute__((interrupt(".rvectors", vector), used));

#define R_PRAGMA_FAST_INTERRUPT(function_name)               extern void function_name(void) __attribute__((interrupt(fast)));
#define R_PRAGMA_STATIC_FAST_INTERRUPT(function_name)        static void function_name(void) __attribute__((interrupt(fast)), used);

#define R_PRAGMA_INTERRUPT_FUNCTION(function_name)           extern void function_name(void) __attribute__((interrupt));
#define R_PRAGMA_STATIC_INTERRUPT_FUNCTION(function_name)    static void function_name(void) __attribute__((interrupt, used));

#define R_ATTRIB_INTERRUPT                                   extern /* only this one because __attribute__((interrupt)) prevents GNURX from generating vector */
#define R_ATTRIB_STATIC_INTERRUPT                            static /* only this one because __attribute__((interrupt, used)) prevents GNURX from generating vector */

#define R_ATTRIB_FAST_INTERRUPT                              extern __attribute__((interrupt(fast)))
#define R_ATTRIB_STATIC_FAST_INTERRUPT                       static __attribute__((interrupt(fast)), used)

#elif defined(__ICCRX__)

#define R_PRAGMA_INTERRUPT(function_name, vect)              R_PRAGMA(vector=vect)\
                                                             extern __interrupt void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT(function_name, vect)       R_PRAGMA(vector=vect)\
                                                             static __interrupt void function_name(void);

#define R_PRAGMA_INTERRUPT_FUNCTION(function_name)           extern __interrupt void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT_FUNCTION(function_name)    static __interrupt void function_name(void);

#define R_PRAGMA_FAST_INTERRUPT(function_name)               extern __fast_interrupt void function_name(void);
#define R_PRAGMA_STATIC_FAST_INTERRUPT(function_name)        static __fast_interrupt void function_name(void);

#define R_ATTRIB_INTERRUPT                                   extern __interrupt
#define R_ATTRIB_STATIC_INTERRUPT                            static __interrupt

#define R_ATTRIB_FAST_INTERRUPT                              extern __fast_interrupt
#define R_ATTRIB_STATIC_FAST_INTERRUPT                       static __fast_interrupt

#endif

//TODO: __attribute__((always_inline)) should be used for GNURX?

#define R_ATTRIB_INLINE                               inline extern
#define R_ATTRIB_STATIC_INLINE                        inline static

#define R_PRAGMA_INLINE(function_prototype)           R_ATTRIB_INLINE function_prototype;
#define R_PRAGMA_STATIC_INLINE(function_prototype)    R_ATTRIB_STATIC_INLINE function_prototype;

#if defined(__CCRX__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_2(bf1, bf2)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_3(bf1, bf2, bf3)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_4(bf1, bf2, bf3, bf4)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, bf31)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
    };\
R_PRAGMA(bit_order)\
}

#elif defined(__GNUC__)

#if defined(__RX_LITTLE_ENDIAN__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_2(bf1, bf2)\
struct {\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_3(bf1, bf2, bf3)\
struct {\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_4(bf1, bf2, bf3, bf4)\
struct {\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11)\
struct {\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12)\
struct {\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13)\
struct {\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14)\
struct {\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15)\
struct {\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21)\
struct {\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22)\
struct {\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23)\
struct {\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24)\
struct {\
        bf24;\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25)\
struct {\
        bf25;\
        bf24;\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
        bf26;\
        bf25;\
        bf24;\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
        bf27;\
        bf26;\
        bf25;\
        bf24;\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
        bf28;\
        bf27;\
        bf26;\
        bf25;\
        bf24;\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
        bf29;\
        bf28;\
        bf27;\
        bf26;\
        bf25;\
        bf24;\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
        bf30;\
        bf29;\
        bf28;\
        bf27;\
        bf26;\
        bf25;\
        bf24;\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, bf31)\
struct {\
        bf31;\
        bf30;\
        bf29;\
        bf28;\
        bf27;\
        bf26;\
        bf25;\
        bf24;\
        bf23;\
        bf22;\
        bf21;\
        bf20;\
        bf19;\
        bf18;\
        bf17;\
        bf16;\
        bf15;\
        bf14;\
        bf13;\
        bf12;\
        bf11;\
        bf10;\
        bf9;\
        bf8;\
        bf7;\
        bf6;\
        bf5;\
        bf4;\
        bf3;\
        bf2;\
        bf1;\
}

#else

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_2(bf1, bf2)\
struct {\
        bf1;\
        bf2;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_3(bf1, bf2, bf3)\
struct {\
        bf1;\
        bf2;\
        bf3;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_4(bf1, bf2, bf3, bf4)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, bf31)\
struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
}

#endif

#elif defined(__ICCRX__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_2(bf1, bf2)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_3(bf1, bf2, bf3)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_4(bf1, bf2, bf3, bf4)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
    };\
R_PRAGMA(bitfield=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, bf31)\
struct {\
R_PRAGMA(bitfield=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
    };\
R_PRAGMA(bitfield=default)\
}

#endif

#if defined(__CCRX__)

#define R_ATTRIB_SECTION_CHANGE_V(section_name)    R_PRAGMA(section section_name)
#define R_ATTRIB_SECTION_CHANGE_F(section_name)    R_PRAGMA(section section_name)
#define R_ATTRIB_SECTION_CHANGE_END                R_PRAGMA(section)

#elif defined(__GNUC__)

#define R_ATTRIB_SECTION_CHANGE_V(section_name)    __attribute__((section(#section_name)))
#define R_ATTRIB_SECTION_CHANGE_F(section_name)    __attribute__((section(#section_name)))
#define R_ATTRIB_SECTION_CHANGE_END                /* none */

#elif defined(__ICCRX__)

#define R_ATTRIB_SECTION_CHANGE_V(section_name)    R_PRAGMA(location=#section_name)\
                                                   __no_init
#define R_ATTRIB_SECTION_CHANGE_F(section_name)    R_PRAGMA(location=#section_name)
#define R_ATTRIB_SECTION_CHANGE_END                /* none */

#endif

/* I guess that following definitions are bad practices because these are redefinitions of keyword, but... */
/* The volatile is a keyword of the C language */
/* The __evenaccess is a keyword of the CC-RX compiler */
/* The __FUNCTION__ is a keyword of GNU/IAR C compilers */

#if defined(__CCRX__)

/* #define volatile        volatile __evenaccess */
#define __FUNCTION__    __func__

#elif defined(__GNUC__)

#define __evenaccess    /* none */

#elif defined(__ICCRX__)

#define __evenaccess    __sfr

#endif

/*
https://gcc-renesas.com/migration-guides/rx/index.html#Compiler_predefined
*/

#if defined(__CCRX__)

/* maybe nothing to do */

#elif defined(__GNUC__)

#define __RX 1

#if defined(__RX_LITTLE_ENDIAN__)
#define __LIT 1
#else
#define __BIG 1
#endif

#elif defined(__ICCRX__)

#define __RX 1

#if defined(__RX_LITTLE_ENDIAN__)
/* #define __LIT 1 */ /* This is already defined by ICCRX. */
#else
/* #define __BIG 1 */ /* This is already defined by ICCRX. */
#endif

#endif

#endif /* SCFGCOMPILERS_H */
