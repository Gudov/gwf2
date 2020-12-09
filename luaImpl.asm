_DATA SEGMENT
	EXTERN lua_load_bytecode_original: PROC
	EXTERN lua_load_original: PROC
	EXTERN warframe_lua_thread_resume_original: PROC
	EXTERN warframe_web_request_original: PROC
	EXTERN warframe_web_request_original_2: PROC
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
		push    rsi
		push    rdi
		push    r14
		sub     rsp, 80h
		mov     r14, rcx
		mov     [r11-20h], rcx

		mov		rax, qword ptr lua_load_original
		add		rax, 1Dh
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

	PUBLIC warframe_web_request_implementation
	warframe_web_request_implementation PROC
		mov     [rsp+20h], rbx
		push    rbp
		push    rsi
		push    rdi
		push    r14
		push    r15
		sub     rsp, 140h

		mov		rax, qword ptr warframe_web_request_original
		add		rax, 13h
		jmp		rax
	warframe_web_request_implementation ENDP

	PUBLIC warframe_web_request_implementation_2
	warframe_web_request_implementation_2 PROC
		mov     [rsp+20h], rbx
		push    rbp
		push    rsi
		push    rdi
		push    r14
		push    r15
		sub     rsp, 140h

		mov		rax, qword ptr warframe_web_request_original_2
		add		rax, 13h
		jmp		rax
	warframe_web_request_implementation_2 ENDP
_TEXT ENDS

END