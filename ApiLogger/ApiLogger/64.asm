extern PrintInfo: proc
extern InstallHook: proc

.data
TmpAddr qword 0

.code

GetReturnAddress proc
	pop rdi
	pop rax
	push rdi
	ret

GetReturnAddress endp

CallBack  proc
	pop rax
	pop rdi
	push rdi
	sub rax,12

	mov TmpAddr,rax

	push rcx
	mov rcx,rax

	push rbp
	mov rbp,rsp
	sub rsp,1024

	call PrintInfo
	mov rsp,rbp
	pop rbp

	pop rcx
	mov rax,TmpAddr
	push rbp
	mov rbp,rsp
	sub rsp,1024
	call rax
	mov rsp,rbp
	pop rbp

	mov rcx,TmpAddr
	mov TmpAddr,rax
	call InstallHook
	mov rax,TmpAddr
	add rsp,32		
	ret

	
	;UINT_PTR RetAddr;
	;RetAddr= GetReturnAddress();
	;RetAddr -= 12;
	;PushRCX();
	;PrintInfo(RetAddr);
	;PopRCX();
	;UINT_PTR Ret;
	;Ret= ((UINT_PTR(*)())RetAddr)();
	;InstallHook
CallBack endp

SetPrinnInfo proc
;	mov PrintInfoAddr,rcx
SetPrinnInfo endp

PushRCX proc
	pop rax
	push rcx
	push rax
	ret

PushRCX endp

PopRCX proc
	pop rax
	pop rcx
	push rax
	ret
PopRCX endp

RestoreStack proc

	pop rax
	pop rcx
	push rax
	ret

RestoreStack endp


end