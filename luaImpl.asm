_DATA SEGMENT
	EXTERN lua_load_bytecode_original: PROC
	EXTERN lua_load_original: PROC
	EXTERN warframe_lua_thread_resume_original: PROC
	EXTERN warframe_web_parse_original: PROC
	EXTERN warframe_moveOrCopyStr_original: PROC
_DATA ENDS

_TEXT SEGMENT
	PUBLIC lua_load_bytecode_implementation
	lua_load_bytecode_implementation PROC
		mov     r11, rsp
		push    rbp
		push    rbx
		push    rsi
		lea     rbp, [r11-128h]
		sub     rsp, 210h

		mov		rax, qword ptr lua_load_bytecode_original
		add		rax, 14h
		jmp		rax
	lua_load_bytecode_implementation ENDP

	PUBLIC lua_load_implementation
	lua_load_implementation PROC
		mov     r11, rsp
		mov     [r11+10h], rbx
		mov     [r11+18h], rbp
		mov     [r11+20h], rsi
		push    rdi
		sub     rsp, 80h
		mov     rbp, rcx

		mov		rax, qword ptr lua_load_original
		add		rax, 17h
		jmp		rax
	lua_load_implementation ENDP

	PUBLIC warframe_lua_thread_resume_implementation
	warframe_lua_thread_resume_implementation PROC
		mov     r11, rsp
		push    rbp
		push    rbx
		push    rdi
		lea     rbp, [r11-5Fh]
		sub     rsp, 0D0h

		mov		rax, qword ptr warframe_lua_thread_resume_original
		add		rax, 11h
		jmp		rax
	warframe_lua_thread_resume_implementation ENDP

	PUBLIC warframe_web_parse_implementation
	warframe_web_parse_implementation PROC
		mov     [rsp+10h], rbx
		mov     [rsp+18h], rbp
		mov     [rsp+20h], rsi
		push    rdi
		push    r14
		push    r15
		sub     rsp, 50h

		mov		rax, qword ptr warframe_web_parse_original
		add		rax, 18h
		jmp		rax
	warframe_web_parse_implementation ENDP

	PUBLIC warframe_moveOrCopyStr_implementation
	warframe_moveOrCopyStr_implementation PROC
		cmp     rdx, rcx
		jz      locret_1
		mov     qword ptr [rsp+8],rbx 
		push    rdi
		sub     rsp, 20h

		mov		rax, qword ptr warframe_moveOrCopyStr_original
		add		rax, 13h
		jmp		rax

		locret_1:
		ret
	warframe_moveOrCopyStr_implementation ENDP
_TEXT ENDS

END