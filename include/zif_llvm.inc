; CP/M-65 Copyright © 2022 David Given
; This file is licensed under the terms of the 2-clause BSD license. Please
; see the COPYING file in the root project directory for the full text.

.altmacro

.set zlabelcount, 0
.set zsp, 0

.set zloopsp, 0
.set zifsp, 0

.macro zproc name
    .text ;\name
    .global \name
    \name:
.endm

.macro zendproc
    .if (zloopsp != 0) || (zifsp != 0)
        .error "Unclosed structure macro"
    .endif
.endm

.macro _zlabel n
    100\n:
.endm

.macro _zlabelref insn, n
    \insn 100\n
.endm

.macro _zgetframe kind, sp
    .set ZF, \kind\sp
.endm

.macro _zframe kind, sp
    .set \kind\sp, zlabelcount
    .set zlabelcount, zlabelcount+1
    _zgetframe \kind, \sp
.endm

.macro _zset stack, sp, name, val
    .set \stack\()_\sp\()_\name, \val
.endm

.macro _zget stack, label, var, insn
    \insn \stack\()_\label\()_\var
.endm

.macro zloop
    .set zloopsp, zloopsp + 1
    _zframe zloopstack, %zloopsp
    _zset zloopstack, %ZF, continue, .
.endm

.macro zendloop
    _zgetframe zloopstack, %zloopsp
    _zget zloopstack, %ZF, continue, jmp
    _zset zloopstack, %ZF, break, .
    .set zloopsp, zloopsp - 1
.endm

.macro _zmake_break cond
    .macro zbreakif_\cond
        _zgetframe zloopstack, %zloopsp
        _zget zloopstack, %ZF, break, b\cond
    .endm
.endm

_zmake_break cs
_zmake_break cc
_zmake_break eq
_zmake_break ne
_zmake_break mi
_zmake_break pl

.macro zrepeat
    .set zloopsp, zloopsp + 1
    _zframe zloopstack, %zloopsp
    _zset zloopstack, %ZF, continue, .
.endm

.macro zuntil_eq
    _zgetframe zloopstack, %zloopsp
    _zget zloopstack, %ZF, continue, bne
    _zset zloopstack, %ZF, break, .
    .set zloopsp, zloopsp - 1
.endm

.macro _zmake_if pos, neg
    .macro zif_\pos
        .set zifsp, zifsp + 1
        _zframe zifstack, %zifsp
        _zget zifstack, %ZF, end, b\neg
    .endm
.endm

_zmake_if eq, ne
_zmake_if ne, eq
_zmake_if cc, cs
_zmake_if cs, cc
_zmake_if pl, mi
_zmake_if mi, pl

.macro zendif
    _zgetframe zifstack, %zifsp
    _zset zifstack, %ZF, end, .
    .set zifsp, zifsp - 1
.endm

; vim: filetype=asm sw=4 ts=4 et

