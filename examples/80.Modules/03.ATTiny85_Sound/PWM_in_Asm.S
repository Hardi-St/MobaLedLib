  .file "PWM_in_Asm.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
 ;  GNU C11 (GCC) version 5.4.0 (avr)
 ;  compiled by GNU C version 4.8.2, GMP version 5.0.2, MPFR version 3.0.0, MPC version 0.9
 ;  GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 ;  options passed:
 ;  -iprefix c:\program files (x86)\arduino\hardware\tools\avr\bin\../lib/gcc/avr/5.4.0/
 ;  C:\Dat\Märklin\Arduino\LEDs_Eisenbahn\Prog_ATTiny85\ATTiny85_Servo\PWM_In_Asm\PWM_in_Asm.c
 ;  -mn-flash=1 -mskip-bug -gstabs -O -fverbose-asm
 ;  options enabled:  -Wmisspelled-isr -faggressive-loop-optimizations
 ;  -fauto-inc-dec -fbranch-count-reg -fchkp-check-incomplete-type
 ;  -fchkp-check-read -fchkp-check-write -fchkp-instrument-calls
 ;  -fchkp-narrow-bounds -fchkp-optimize -fchkp-store-bounds
 ;  -fchkp-use-static-bounds -fchkp-use-static-const-bounds
 ;  -fchkp-use-wrappers -fcombine-stack-adjustments -fcommon -fcompare-elim
 ;  -fcprop-registers -fdefer-pop -fdwarf2-cfi-asm -fearly-inlining
 ;  -feliminate-unused-debug-types -fforward-propagate -ffunction-cse
 ;  -fgcse-lm -fgnu-runtime -fgnu-unique -fguess-branch-probability -fident
 ;  -fif-conversion -fif-conversion2 -finline -finline-atomics
 ;  -finline-functions-called-once -fipa-profile -fipa-pure-const
 ;  -fipa-reference -fira-hoist-pressure -fira-share-save-slots
 ;  -fira-share-spill-slots -fivopts -fkeep-static-consts
 ;  -fleading-underscore -flifetime-dse -flto-odr-type-merging -fmath-errno
 ;  -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
 ;  -fomit-frame-pointer -fpeephole -fprefetch-loop-arrays
 ;  -freg-struct-return -fsched-critical-path-heuristic
 ;  -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
 ;  -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
 ;  -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fschedule-fusion
 ;  -fsemantic-interposition -fshow-column -fshrink-wrap -fsigned-zeros
 ;  -fsplit-ivs-in-unroller -fsplit-wide-types -fssa-phiopt -fstdarg-opt
 ;  -fstrict-volatile-bitfields -fsync-libcalls -ftoplevel-reorder
 ;  -ftrapping-math -ftree-bit-ccp -ftree-ccp -ftree-ch
 ;  -ftree-coalesce-vars -ftree-copy-prop -ftree-copyrename -ftree-dce
 ;  -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
 ;  -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
 ;  -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop
 ;  -ftree-pta -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-slsr
 ;  -ftree-sra -ftree-ter -funit-at-a-time -fverbose-asm
 ;  -fzero-initialized-in-bss -mskip-bug

  .text
.Ltext0:
.global PWM_in_Raw
PWM_in_Raw:
// *** // Compile this module with
// *** //   cd "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin"
// *** //   avr-gcc.exe -gstabs -Wa,-ahlmsd="C:\Dat\Märklin\Arduino\LEDs_Eisenbahn\Prog_ATTiny85\ATTiny85_
// *** //
// *** // and manually convert the generated PWM_in_Asm.lst to PWM_in_Asm.S
// *** //
// *** // If the optimization -O2 is used the program is 8-10% faster. But unfortunately only some
// *** // of the while() loops are faster ;-(


// *** //
// *** //  PWM  Period  with optimization -O2
// *** //  25%  2078
// *** //  96%  1930
// *** //
// *** // Thereby the period is different if the PWM signal changes ;-((
// *** //
// *** // Therefore the optimization -O is used.
// *** //
// *** // Maybe this could be improved by modifieing the ASM code manually.
// *** //
// *** // GCC: (GNU) 5.4.0
// ***
// *** typedef unsigned char uint8_t;
// ***
// *** //-------------------------------------------------------------------------------------------------
// *** unsigned int PWM_in_Raw(volatile uint8_t *port, uint8_t bit, uint8_t stateMask, unsigned int maxloo
// *** //-------------------------------------------------------------------------------------------------
// *** // Try to measure also the opposite pulse to be able to calculate the PWM. If the timeout is to sho
// *** // opposite pulse may not measured. It depends from the time the function is called if the opposite
// *** // is measured.
// *** // If the opposite pulse can't be measured Period is 0.
// *** // => The period has to be stored outside to use prior values in this case.
// *** //
// *** // The "pictures" below show the situation when stateMask is xxx1xxx which is used to detect HIGH p
// *** // If LOW pulses are detected stateMask is set to xxx0xxx. In this case the pictures hafe do be fli
// *** {
.LM0:
.LFBB1:
    push r14   ;   ;  141 pushqi1/1 [length = 1]
    push r15   ;   ;  142 pushqi1/1 [length = 1]
    push r16   ;   ;  143 pushqi1/1 [length = 1]
    push r17   ;   ;  144 pushqi1/1 [length = 1]
    push r28   ;   ;  145 pushqi1/1 [length = 1]
    push r29   ;   ;  146 pushqi1/1 [length = 1]
/* prologue: function */
/* frame size = 0 */
/* stack size = 6 */
.L__stack_usage = 6
    mov r26,r24  ;  port, port   ;  2 *movhi/1  [length = 2]
    mov r27,r25  ;  port, port
// ***     unsigned int Start_maxloop = maxloops;
// ***     unsigned int End_Prev = 0;
// ***     unsigned int Start_Pulse;
// ***     unsigned int End_Pulse;
// ***     *Period = 0;
.LM1:
    mov r30,r16  ; , Period  ;  134 *movhi/1  [length = 2]
    mov r31,r17  ; , Period
    std Z+1,__zero_reg__   ;  *Period_10(D),   ;  17  *movhi/4  [length = 2]
    st Z,__zero_reg__  ;  *Period_10(D),
// ***
// ***     // wait for any previous pulse to end    // ====.
// ***     while ((*port & bit) == stateMask)       //     #
.LM2:
    mov r30,r18  ;  maxloops, maxloops   ;  8 *movhi/1  [length = 2]
    mov r31,r19  ;  maxloops, maxloops


    rjmp .L2   ;   ;  116 jump  [length = 1]
.L4:
// ***         if (--maxloops == 0)                 //     '=====
.LM3:
    sbiw r30,1   ;  maxloops,  ;  21  addhi3_clobber/1  [length = 1]
    brne .+2   ;   ;  23  branch  [length = 2]
    rjmp .L11  ;
.L2:
// ***         if (--maxloops == 0)                 //     '=====
.LM4:
    ld r25,X   ;  D.1634, *port_12(D)  ;  26  movqi_insn/4  [length = 1]
    and r25,r22  ;  D.1634, bit  ;  28  andqi3/1  [length = 1]
    cp r25,r20   ;  D.1634, stateMask  ;  29  *cmpqi/2  [length = 1]
    breq .L4   ; ,   ;  30  branch  [length = 1]
    mov r28,r30  ;  maxloops, maxloops   ;  9 *movhi/1  [length = 2]
    mov r29,r31  ;  maxloops, maxloops
    rjmp .L5   ;   ;  118 jump  [length = 1]
.L6:
// ***             return 0;
// ***
// ***     End_Prev = maxloops;                     //             If End_Prev < Start_maxloop => we have
// ***
// ***     // wait for the pulse to start           // ....          .=====
// ***     while ((*port & bit) != stateMask)       //     :         #
// ***         if (--maxloops == 0)                 //     ......===='
.LM5:
    sbiw r28,1   ;  maxloops,  ;  35  addhi3_clobber/1  [length = 1]
    brne .+2   ;   ;  37  branch  [length = 1]
    rjmp .L12  ;
.L5:
// ***         if (--maxloops == 0)                 //     ......===='
.LM6:
    ld r25,X   ;  D.1634, *port_12(D)  ;  40  movqi_insn/4  [length = 1]
    and r25,r22  ;  D.1634, bit  ;  42  andqi3/1  [length = 1]
    cpse r20,r25   ;  stateMask, D.1634  ;  43  enable_interrupt-3  [length = 1]
    rjmp .L6   ;
    mov r14,r28  ;  maxloops, maxloops   ;  10  *movhi/1  [length = 2]
    mov r15,r29  ;  maxloops, maxloops
    rjmp .L7   ;   ;  120 jump  [length = 1]
.L8:
// ***             return 0;
// ***
// ***     Start_Pulse = maxloops;
// ***                                              //             End_Prev - maxloops = duration before t
// ***     // wait for the pulse to stop            // .....         ......====.
// ***     while ((*port & bit) == stateMask)       //     :         :         #
// ***         if (--maxloops == 0)                 //     '.........'         '=====
.LM7:
    ldi r24,1  ; ,   ;  49  addhi3_clobber/3  [length = 3]
    sub r14,r24  ;  maxloops,
    sbc r15,__zero_reg__   ;  maxloops
    breq .L13  ; ,   ;  51  branch  [length = 1]
.L7:
// ***         if (--maxloops == 0)                 //     '.........'         '=====
.LM8:
    ld r24,X   ;  D.1634, *port_12(D)  ;  54  movqi_insn/4  [length = 1]
    and r24,r22  ;  D.1634, bit  ;  56  andqi3/1  [length = 1]


    cp r20,r24   ;  stateMask, D.1634  ;  57  *cmpqi/2  [length = 1]
    breq .L8   ; ,   ;  58  branch  [length = 1]
// ***             return 0;
// ***
// ***     if (End_Prev < Start_maxloop)
.LM9:
    cp r30,r18   ;  maxloops, maxloops   ;  60  *cmphi/3  [length = 2]
    cpc r31,r19  ;  maxloops, maxloops
    brsh .L14  ; ,   ;  61  branch  [length = 1]
// ***          { // we have detected the falling edge at the beginning
// ***          *Period = End_Prev - maxloops;
.LM10:
    sub r30,r14  ;  D.1633, maxloops   ;  63  subhi3/1  [length = 2]
    sbc r31,r15  ;  D.1633, maxloops
    mov r26,r16  ; , Period  ;  135 *movhi/1  [length = 2]
    mov r27,r17  ; , Period
    st X+,r30  ;  D.1633   ;  64  *movhi/4  [length = 2]
    st X,r31   ;  D.1633
// ***          return Start_Pulse - maxloops;
.LM11:
    mov r24,r28  ;  D.1633, maxloops   ;  136 *movhi/1  [length = 2]
    mov r25,r29  ;  D.1633, maxloops
    sub r24,r14  ;  D.1633, maxloops   ;  65  subhi3/1  [length = 2]
    sbc r25,r15  ;  D.1633, maxloops
    rjmp .L3   ;   ;  122 jump  [length = 1]
.L10:
// ***          }
// ***
// ***     // we didn't detect the falling edge at the beginning
// ***     End_Pulse = maxloops;
// ***     // wait for the next pulse to start      // .....         ...........         .=====
// ***     while ((*port & bit) != stateMask)       //     :         :         :         #
// ***         if (--maxloops == 0)                 //     '.........'         '.....===='
.LM12:
    subi r18,1   ;  maxloops,  ;  69  addhi3_clobber/2  [length = 2]
    sbc r19,__zero_reg__   ;  maxloops
    brne .L9   ; ,   ;  71  branch  [length = 1]
// ***             return Start_Pulse - End_Pulse;
.LM13:
    mov r24,r28  ;  D.1633, maxloops   ;  137 *movhi/1  [length = 2]
    mov r25,r29  ;  D.1633, maxloops
    sub r24,r14  ;  D.1633, maxloops   ;  73  subhi3/1  [length = 2]
    sbc r25,r15  ;  D.1633, maxloops
    rjmp .L3   ;   ;  124 jump  [length = 1]
.L14:
    mov r18,r14  ;  maxloops, maxloops   ;  11  *movhi/1  [length = 2]
    mov r19,r15  ;  maxloops, maxloops
.L9:
// ***         if (--maxloops == 0)                 //     '.........'         '.....===='
.LM14:
    ld r24,X   ;  D.1634, *port_12(D)  ;  78  movqi_insn/4  [length = 1]
    and r24,r22  ;  D.1634, bit  ;  80  andqi3/1  [length = 1]
    cpse r20,r24   ;  stateMask, D.1634  ;  81  enable_interrupt-3  [length = 1]
    rjmp .L10  ;
// ***
// ***     // We have detected the next pulse
// ***     *Period = Start_Pulse - maxloops;


.LM15:
    mov r24,r28  ;  D.1633, maxloops   ;  138 *movhi/1  [length = 2]
    mov r25,r29  ;  D.1633, maxloops
    sub r24,r18  ;  D.1633, maxloops   ;  84  subhi3/1  [length = 2]
    sbc r25,r19  ;  D.1633, maxloops
    mov r30,r16  ; , Period  ;  139 *movhi/1  [length = 2]
    mov r31,r17  ; , Period
    std Z+1,r25  ;  *Period_10(D), D.1633  ;  85  *movhi/4  [length = 2]
    st Z,r24   ;  *Period_10(D), D.1633
// ***     return Start_Pulse - End_Pulse;
.LM16:
    mov r24,r28  ;  D.1633, maxloops   ;  140 *movhi/1  [length = 2]
    mov r25,r29  ;  D.1633, maxloops
    sub r24,r14  ;  D.1633, maxloops   ;  86  subhi3/1  [length = 2]
    sbc r25,r15  ;  D.1633, maxloops
    rjmp .L3   ;   ;  126 jump  [length = 1]
.L11:
// ***
.LM17:
    ldi r24,0  ;  D.1633   ;  12  *movhi/2  [length = 2]
    ldi r25,0  ;  D.1633
    rjmp .L3   ;   ;  128 jump  [length = 1]
.L12:
// ***
.LM18:
    ldi r24,0  ;  D.1633   ;  13  *movhi/2  [length = 2]
    ldi r25,0  ;  D.1633
    rjmp .L3   ;   ;  130 jump  [length = 1]
.L13:
// ***
.LM19:
    ldi r24,0  ;  D.1633   ;  14  *movhi/2  [length = 2]
    ldi r25,0  ;  D.1633
.L3:
/* epilogue start */
// *** }
.LM20:
    pop r29  ;   ;  149 popqi [length = 1]
    pop r28  ;   ;  150 popqi [length = 1]
    pop r17  ;   ;  151 popqi [length = 1]
    pop r16  ;   ;  152 popqi [length = 1]
    pop r15  ;   ;  153 popqi [length = 1]
    pop r14  ;   ;  154 popqi [length = 1]
    ret  ;  155 return_from_epilogue  [length = 1]
.Lscope1:
.Letext0:
  .ident  "GCC: (GNU) 5.4.0"


