	.file	"mysh.c"
	.text
.globl exit_error
	.type	exit_error, @function
exit_error:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$1696624193, -32(%rbp)
	movl	$1919906418, -28(%rbp)
	movl	$1935763488, -24(%rbp)
	movl	$1667460896, -20(%rbp)
	movl	$1701999221, -16(%rbp)
	movl	$2660, -12(%rbp)
	movl	$0, -8(%rbp)
	movw	$0, -4(%rbp)
	leaq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	strlen
	movq	%rax, %rdx
	leaq	-32(%rbp), %rax
	movq	%rax, %rsi
	movl	$2, %edi
	call	write
	movl	$1, %edi
	call	exit
	.cfi_endproc
.LFE0:
	.size	exit_error, .-exit_error
.globl error
	.type	error, @function
error:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$1696624193, -32(%rbp)
	movl	$1919906418, -28(%rbp)
	movl	$1935763488, -24(%rbp)
	movl	$1667460896, -20(%rbp)
	movl	$1701999221, -16(%rbp)
	movl	$2660, -12(%rbp)
	movl	$0, -8(%rbp)
	movw	$0, -4(%rbp)
	leaq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	strlen
	movq	%rax, %rdx
	leaq	-32(%rbp), %rax
	movq	%rax, %rsi
	movl	$2, %edi
	call	write
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	error, .-error
.globl arg_checker
	.type	arg_checker, @function
arg_checker:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$1, %eax
	jne	.L6
	movl	$0, %eax
	jmp	.L7
.L6:
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$2, %eax
	jne	.L8
	movl	$1, %eax
	jmp	.L7
.L8:
	movl	$-1, %eax
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	arg_checker, .-arg_checker
.globl cmdlen_check
	.type	cmdlen_check, @function
cmdlen_check:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	addq	$8, %rax
	movq	%rax, %rdi
	call	strlen
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	cmpl	$512, -4(%rbp)
	jbe	.L11
	movl	$-1, %eax
	jmp	.L12
.L11:
	movl	$0, %eax
.L12:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	cmdlen_check, .-cmdlen_check
.globl sh_prompt
	.type	sh_prompt, @function
sh_prompt:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -56(%rbp)
	movl	$1752398189, -48(%rbp)
	movl	$8254, -44(%rbp)
	movq	$0, -40(%rbp)
	movl	$0, -32(%rbp)
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	strlen
	movq	%rax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	write
	movq	stdout(%rip), %rax
	movq	%rax, %rdi
	call	fflush
	movq	stdin(%rip), %rax
	movq	-56(%rbp), %rdx
	leaq	8(%rdx), %rcx
	movq	%rax, %rdx
	movl	$1000, %esi
	movq	%rcx, %rdi
	call	fgets
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.L15
	movl	$0, %edi
	call	exit
.L15:
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	cmdlen_check
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jns	.L16
	movq	-56(%rbp), %rax
	movl	$1, 1176(%rax)
.L16:
	movq	-56(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	sh_prompt, .-sh_prompt
	.section	.rodata
.LC0:
	.string	">"
.LC1:
	.string	"exit"
.LC2:
	.string	"pwd"
.LC3:
	.string	"cd"
.LC4:
	.string	"wait"
.LC5:
	.string	".py"
	.text
.globl cmd_parse
	.type	cmd_parse, @function
cmd_parse:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$160, %rsp
	movq	%rdi, -152(%rbp)
	movl	$0, -56(%rbp)
	movl	$0, -52(%rbp)
	movq	-152(%rbp), %rax
	addq	$8, %rax
	movl	$62, %esi
	movq	%rax, %rdi
	call	strchr
	movq	%rax, -72(%rbp)
	jmp	.L19
.L20:
	addl	$1, -56(%rbp)
	movq	-72(%rbp), %rax
	addq	$1, %rax
	movl	$62, %esi
	movq	%rax, %rdi
	call	strchr
	movq	%rax, -72(%rbp)
.L19:
	cmpq	$0, -72(%rbp)
	jne	.L20
	cmpl	$1, -56(%rbp)
	jle	.L21
	movq	-152(%rbp), %rax
	movl	$1, 1176(%rax)
.L21:
	movq	-152(%rbp), %rax
	addq	$8, %rax
	movl	$38, %esi
	movq	%rax, %rdi
	call	strchr
	movq	%rax, -64(%rbp)
	jmp	.L22
.L23:
	addl	$1, -52(%rbp)
	movq	-64(%rbp), %rax
	addq	$1, %rax
	movl	$38, %esi
	movq	%rax, %rdi
	call	strchr
	movq	%rax, -64(%rbp)
.L22:
	cmpq	$0, -64(%rbp)
	jne	.L23
	cmpl	$1, -52(%rbp)
	jle	.L24
	movq	-152(%rbp), %rax
	movl	$1, 1176(%rax)
	jmp	.L25
.L24:
	cmpl	$1, -52(%rbp)
	jne	.L25
	movq	-152(%rbp), %rax
	movl	$1, 1196(%rax)
.L25:
	movl	$0, -12(%rbp)
	movl	$218703392, -80(%rbp)
	movw	$2826, -76(%rbp)
	movb	$0, -74(%rbp)
	movl	$168626494, -96(%rbp)
	movw	$11, -92(%rbp)
	movl	$1, -8(%rbp)
	movq	-152(%rbp), %rax
	addq	$8, %rax
	movq	%rax, -48(%rbp)
.L39:
	leaq	-80(%rbp), %rcx
	leaq	-104(%rbp), %rdx
	movq	-48(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	strtok_r
	movq	%rax, -32(%rbp)
	cmpq	$0, -32(%rbp)
	je	.L45
.L26:
	movq	-32(%rbp), %rax
	movzbl	(%rax), %edx
	movl	$.LC0, %eax
	movzbl	(%rax), %eax
	cmpb	%al, %dl
	jne	.L28
	leaq	-80(%rbp), %rcx
	leaq	-104(%rbp), %rdx
	movq	-48(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	strtok_r
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L29
	leaq	-96(%rbp), %rcx
	leaq	-112(%rbp), %rdx
	movq	-32(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	strtok_r
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L30
	movq	-152(%rbp), %rax
	movl	$1, 1176(%rax)
	jmp	.L27
.L30:
	movq	-152(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, 1184(%rax)
	movq	-152(%rbp), %rax
	movl	$1, 1192(%rax)
	jmp	.L31
.L29:
	movq	-152(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, 1184(%rax)
	movq	-152(%rbp), %rax
	movl	$1, 1192(%rax)
	jmp	.L31
.L28:
	movq	-32(%rbp), %rax
	movl	$62, %esi
	movq	%rax, %rdi
	call	strchr
	testq	%rax, %rax
	je	.L32
	movq	-32(%rbp), %rax
	movq	%rax, -40(%rbp)
.L37:
	leaq	-96(%rbp), %rcx
	leaq	-112(%rbp), %rdx
	movq	-40(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	strtok_r
	movq	%rax, -24(%rbp)
	movq	-152(%rbp), %rax
	movl	1192(%rax), %eax
	cmpl	$1, %eax
	jne	.L33
	cmpq	$0, -24(%rbp)
	jne	.L33
	leaq	-80(%rbp), %rax
	leaq	-104(%rbp), %rdx
	movq	%rax, %rsi
	movl	$0, %edi
	call	strtok_r
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L34
	movq	-152(%rbp), %rax
	movl	$1, 1176(%rax)
	jmp	.L31
.L34:
	movq	-152(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, 1184(%rax)
	jmp	.L31
.L33:
	movq	-152(%rbp), %rax
	movl	1192(%rax), %eax
	cmpl	$1, %eax
	jne	.L36
	movq	-152(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, 1184(%rax)
	nop
	jmp	.L31
.L36:
	movl	-12(%rbp), %edx
	movq	-152(%rbp), %rax
	movslq	%edx, %rdx
	leaq	126(%rdx), %rcx
	movq	-24(%rbp), %rdx
	movq	%rdx, (%rax,%rcx,8)
	movq	-152(%rbp), %rax
	movl	$1, 1192(%rax)
	addl	$1, -12(%rbp)
	movq	$0, -40(%rbp)
	jmp	.L37
.L32:
	movq	-152(%rbp), %rax
	movl	1192(%rax), %eax
	cmpl	$1, %eax
	jne	.L38
	movq	-152(%rbp), %rax
	movl	$1, 1176(%rax)
	jmp	.L27
.L38:
	movl	-12(%rbp), %edx
	movq	-152(%rbp), %rax
	movslq	%edx, %rdx
	leaq	126(%rdx), %rcx
	movq	-32(%rbp), %rdx
	movq	%rdx, (%rax,%rcx,8)
	addl	$1, -12(%rbp)
.L31:
	movq	$0, -48(%rbp)
	jmp	.L39
.L45:
	nop
.L27:
	movl	-12(%rbp), %edx
	movq	-152(%rbp), %rax
	movslq	%edx, %rdx
	addq	$126, %rdx
	movq	$0, (%rax,%rdx,8)
	movq	-152(%rbp), %rax
	movl	-12(%rbp), %edx
	movl	%edx, 1168(%rax)
	movq	-152(%rbp), %rax
	movl	1168(%rax), %eax
	testl	%eax, %eax
	je	.L40
	movq	$.LC1, -144(%rbp)
	movq	$.LC2, -136(%rbp)
	movq	$.LC3, -128(%rbp)
	movq	$.LC4, -120(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L41
.L43:
	movq	-152(%rbp), %rax
	movq	1008(%rax), %rcx
	movl	-4(%rbp), %eax
	cltq
	movq	-144(%rbp,%rax,8), %rax
	movl	$4, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	strncmp
	testl	%eax, %eax
	jne	.L42
	movq	-152(%rbp), %rax
	movq	1008(%rax), %rdx
	movq	-152(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-152(%rbp), %rax
	movl	$1, 1172(%rax)
.L42:
	addl	$1, -4(%rbp)
.L41:
	cmpl	$3, -4(%rbp)
	jle	.L43
	movq	-152(%rbp), %rax
	movq	1008(%rax), %rax
	movl	$.LC5, %esi
	movq	%rax, %rdi
	call	strstr
	testq	%rax, %rax
	je	.L40
	movq	-152(%rbp), %rax
	movq	$.LC5, (%rax)
	movq	-152(%rbp), %rax
	movl	$1, 1172(%rax)
.L40:
	movq	-152(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	cmd_parse, .-cmd_parse
	.section	.rodata
.LC6:
	.string	"."
.LC7:
	.string	"\n"
.LC8:
	.string	""
.LC9:
	.string	"/usr/bin/python"
	.text
.globl exec_cmd
	.type	exec_cmd, @function
exec_cmd:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	subq	$1312, %rsp
	movq	%rdi, -1320(%rbp)
	.cfi_offset 3, -32
	.cfi_offset 12, -24
	call	fork
	movl	%eax, -76(%rbp)
	cmpl	$0, -76(%rbp)
	jns	.L47
	movl	$0, %eax
	call	error
	jmp	.L48
.L47:
	cmpl	$0, -76(%rbp)
	jne	.L49
	movq	-1320(%rbp), %rax
	movl	1172(%rax), %eax
	testl	%eax, %eax
	je	.L50
	movq	-1320(%rbp), %rax
	movq	(%rax), %rax
	movl	$.LC2, %esi
	movq	%rax, %rdi
	call	strcmp
	testl	%eax, %eax
	jne	.L51
	movq	-1320(%rbp), %rax
	leaq	-1312(%rbp), %rdx
	movq	%rax, %rbx
	movl	$150, %eax
	movq	%rdx, %rdi
	movq	%rbx, %rsi
	movq	%rax, %rcx
	rep movsq
	movl	-144(%rbp), %eax
	cmpl	$1, %eax
	jle	.L52
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L52:
	movl	-120(%rbp), %eax
	cmpl	$1, %eax
	jne	.L53
	movl	$1, %edi
	call	close
	movl	%eax, -48(%rbp)
	cmpl	$0, -48(%rbp)
	jns	.L54
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L54:
	movq	-128(%rbp), %rax
	movl	$448, %edx
	movl	$578, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open
	movl	%eax, -44(%rbp)
	cmpl	$0, -44(%rbp)
	jns	.L53
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L53:
	movl	$4, %esi
	movl	$.LC6, %edi
	call	pathconf
	movq	%rax, -72(%rbp)
	movq	-72(%rbp), %rax
	movq	%rax, %rdi
	call	malloc
	movq	%rax, -64(%rbp)
	cmpq	$0, -64(%rbp)
	je	.L55
	movq	-72(%rbp), %rdx
	movq	-64(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	getcwd
	movq	%rax, -56(%rbp)
	movl	$.LC7, %edx
	movq	-56(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcat
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	strlen
	movq	%rax, %rdx
	movq	-56(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	write
	movl	$0, %edi
	call	exit
.L55:
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L51:
	movq	-1320(%rbp), %rax
	movq	(%rax), %rax
	movl	$.LC5, %esi
	movq	%rax, %rdi
	call	strcmp
	testl	%eax, %eax
	jne	.L48
	movq	%rsp, %rax
	movq	%rax, %r12
	movq	-1320(%rbp), %rax
	leaq	-1312(%rbp), %rdx
	movq	%rax, %rbx
	movl	$150, %eax
	movq	%rdx, %rdi
	movq	%rbx, %rsi
	movq	%rax, %rcx
	rep movsq
	movl	-144(%rbp), %eax
	addl	$2, %eax
	movslq	%eax, %rdx
	subq	$1, %rdx
	movq	%rdx, -96(%rbp)
	cltq
	salq	$3, %rax
	addq	$15, %rax
	addq	$15, %rax
	shrq	$4, %rax
	salq	$4, %rax
	subq	%rax, %rsp
	movq	%rsp, %rax
	addq	$15, %rax
	shrq	$4, %rax
	salq	$4, %rax
	movq	%rax, -88(%rbp)
	movq	-88(%rbp), %rax
	movq	$.LC8, (%rax)
	movl	-120(%rbp), %eax
	cmpl	$1, %eax
	jne	.L56
	movl	$1, %edi
	call	close
	movl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	jns	.L57
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L57:
	movq	-128(%rbp), %rax
	movl	$448, %edx
	movl	$578, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open
	movl	%eax, -32(%rbp)
	cmpl	$0, -32(%rbp)
	jns	.L56
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L56:
	movl	$0, -40(%rbp)
	jmp	.L58
.L59:
	movl	-40(%rbp), %eax
	leal	1(%rax), %edx
	movl	-40(%rbp), %eax
	cltq
	addq	$126, %rax
	movq	-1312(%rbp,%rax,8), %rcx
	movq	-88(%rbp), %rax
	movslq	%edx, %rdx
	movq	%rcx, (%rax,%rdx,8)
	addl	$1, -40(%rbp)
.L58:
	movl	-144(%rbp), %eax
	cmpl	-40(%rbp), %eax
	jg	.L59
	movl	-144(%rbp), %eax
	leal	1(%rax), %edx
	movq	-88(%rbp), %rax
	movslq	%edx, %rdx
	movq	$0, (%rax,%rdx,8)
	movq	-88(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC9, %edi
	call	execvp
	movq	%r12, %rsp
	jmp	.L48
.L50:
	movq	-1320(%rbp), %rax
	leaq	-1312(%rbp), %rdx
	movq	%rax, %rbx
	movl	$150, %eax
	movq	%rdx, %rdi
	movq	%rbx, %rsi
	movq	%rax, %rcx
	rep movsq
	movl	-120(%rbp), %eax
	cmpl	$1, %eax
	jne	.L60
	movl	$1, %edi
	call	close
	movl	%eax, -28(%rbp)
	cmpl	$0, -28(%rbp)
	jns	.L61
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L61:
	movq	-128(%rbp), %rax
	movl	$448, %edx
	movl	$578, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open
	movl	%eax, -24(%rbp)
	cmpl	$0, -24(%rbp)
	jns	.L60
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L60:
	movq	-304(%rbp), %rax
	leaq	-1312(%rbp), %rdx
	addq	$1008, %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	execvp
	testl	%eax, %eax
	jns	.L48
	movl	$0, %eax
	call	error
	movl	$1, %edi
	call	exit
.L49:
	movq	-1320(%rbp), %rax
	movl	1196(%rax), %eax
	cmpl	$1, %eax
	je	.L62
	movl	$0, %eax
	leaq	-100(%rbp), %rax
	movq	%rax, %rdi
	call	wait
	movl	%eax, -20(%rbp)
	cmpl	$-1, -20(%rbp)
	jne	.L63
	movl	$-1, %eax
	jmp	.L64
.L63:
	movl	-100(%rbp), %eax
	movl	$0, -80(%rbp)
	movl	%eax, -80(%rbp)
	jmp	.L48
.L62:
	movq	-1320(%rbp), %rax
	movl	$0, 1196(%rax)
.L48:
	movl	$0, %eax
.L64:
	leaq	-16(%rbp), %rsp
	addq	$0, %rsp
	popq	%rbx
	popq	%r12
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	exec_cmd, .-exec_cmd
	.section	.rodata
.LC10:
	.string	"HOME"
	.text
.globl shell_mode
	.type	shell_mode, @function
shell_mode:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$1272, %rsp
	movq	%rdi, -1272(%rbp)
	movq	-1272(%rbp), %rax
	movq	%rax, %rdi
	.cfi_offset 3, -24
	call	cmd_parse
	movq	%rax, -1272(%rbp)
	movq	-1272(%rbp), %rax
	movl	1168(%rax), %eax
	testl	%eax, %eax
	je	.L67
	movq	-1272(%rbp), %rax
	movl	1176(%rax), %eax
	cmpl	$1, %eax
	jne	.L68
	movl	$-1, %eax
	jmp	.L69
.L68:
	movq	-1272(%rbp), %rax
	movl	1172(%rax), %eax
	testl	%eax, %eax
	jne	.L70
	movq	-1272(%rbp), %rax
	movq	%rax, %rdi
	call	exec_cmd
	movl	%eax, -60(%rbp)
	cmpl	$0, -60(%rbp)
	je	.L67
	movl	$-1, %eax
	jmp	.L69
.L70:
	movq	-1272(%rbp), %rax
	movq	(%rax), %rax
	movl	$.LC1, %esi
	movq	%rax, %rdi
	call	strcmp
	testl	%eax, %eax
	jne	.L71
	movq	-1272(%rbp), %rax
	movl	1168(%rax), %eax
	cmpl	$1, %eax
	jle	.L72
	movl	$-1, %eax
	jmp	.L69
.L72:
	movl	$0, %edi
	call	exit
.L71:
	movq	-1272(%rbp), %rax
	movq	(%rax), %rax
	movl	$.LC3, %esi
	movq	%rax, %rdi
	call	strcmp
	testl	%eax, %eax
	jne	.L73
	movq	-1272(%rbp), %rax
	leaq	-1264(%rbp), %rdx
	movq	%rax, %rbx
	movl	$150, %eax
	movq	%rdx, %rdi
	movq	%rbx, %rsi
	movq	%rax, %rcx
	rep movsq
	movl	-72(%rbp), %eax
	cmpl	$1, %eax
	jne	.L74
	movl	$1, %edi
	call	close
	movl	%eax, -56(%rbp)
	cmpl	$0, -56(%rbp)
	jns	.L75
	movl	$-1, %eax
	jmp	.L69
.L75:
	movq	-80(%rbp), %rax
	movl	$448, %edx
	movl	$578, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open
	movl	%eax, -52(%rbp)
	cmpl	$0, -52(%rbp)
	jns	.L74
	movl	$-1, %eax
	jmp	.L69
.L74:
	movl	-96(%rbp), %eax
	cmpl	$1, %eax
	jne	.L76
	movl	$.LC10, %edi
	call	getenv
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	chdir
	movl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	jns	.L67
	movl	$-1, %eax
	jmp	.L69
.L76:
	movq	-248(%rbp), %rax
	movq	%rax, %rdi
	call	chdir
	movl	%eax, -32(%rbp)
	cmpl	$0, -32(%rbp)
	jns	.L67
	movl	$-1, %eax
	jmp	.L69
.L73:
	movq	-1272(%rbp), %rax
	movq	(%rax), %rax
	movl	$.LC4, %esi
	movq	%rax, %rdi
	call	strcmp
	testl	%eax, %eax
	jne	.L77
	movq	-1272(%rbp), %rax
	leaq	-1264(%rbp), %rdx
	movq	%rax, %rbx
	movl	$150, %eax
	movq	%rdx, %rdi
	movq	%rbx, %rsi
	movq	%rax, %rcx
	rep movsq
	movl	-96(%rbp), %eax
	cmpl	$1, %eax
	jle	.L78
	movl	$0, %eax
	call	error
	movl	$0, %edi
	call	exit
.L78:
	movl	-72(%rbp), %eax
	cmpl	$1, %eax
	jne	.L85
	movl	$1, %edi
	call	close
	movl	%eax, -28(%rbp)
	cmpl	$0, -28(%rbp)
	jns	.L80
	movl	$-1, %eax
	jmp	.L69
.L80:
	movq	-80(%rbp), %rax
	movl	$448, %edx
	movl	$578, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open
	movl	%eax, -24(%rbp)
	cmpl	$0, -24(%rbp)
	jns	.L86
	movl	$-1, %eax
	jmp	.L69
.L83:
	call	__errno_location
	movl	(%rax), %eax
	cmpl	$10, %eax
	je	.L87
	jmp	.L81
.L85:
	nop
	jmp	.L81
.L86:
	nop
.L81:
	movl	$0, %edx
	movl	$0, %esi
	movl	$-1, %edi
	call	waitpid
	testl	%eax, %eax
	jne	.L83
	jmp	.L67
.L77:
	movq	-1272(%rbp), %rax
	movq	%rax, %rdi
	call	exec_cmd
	movl	%eax, -20(%rbp)
	cmpl	$0, -20(%rbp)
	je	.L67
	movl	$-1, %eax
	jmp	.L69
.L87:
	nop
.L67:
	movl	$0, %eax
.L69:
	addq	$1272, %rsp
	popq	%rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	shell_mode, .-shell_mode
	.section	.rodata
.LC11:
	.string	"r"
	.text
.globl batch_mode
	.type	batch_mode, @function
batch_mode:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$1288, %rsp
	movq	%rdi, -1288(%rbp)
	movq	-1288(%rbp), %rax
	movq	%rax, -56(%rbp)
	movl	$.LC11, %edx
	movq	-56(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	.cfi_offset 3, -24
	call	fopen
	movq	%rax, -48(%rbp)
	cmpq	$0, -48(%rbp)
	jne	.L89
	movl	$-1, %eax
	jmp	.L90
.L89:
	movq	$0, -64(%rbp)
	movq	$0, -72(%rbp)
	jmp	.L91
.L94:
	leaq	-1280(%rbp), %rbx
	movl	$0, %eax
	movl	$150, %edx
	movq	%rbx, %rdi
	movq	%rdx, %rcx
	rep stosq
	leaq	-1280(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-64(%rbp), %rax
	movq	-32(%rbp), %rdx
	addq	$8, %rdx
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	strcpy
	testq	%rax, %rax
	jne	.L92
	movq	-32(%rbp), %rax
	movl	$1, 1176(%rax)
.L92:
	movq	-32(%rbp), %rax
	addq	$8, %rax
	movq	%rax, %rdi
	call	strlen
	movq	-32(%rbp), %rdx
	leaq	8(%rdx), %rcx
	movq	%rax, %rdx
	movq	%rcx, %rsi
	movl	$1, %edi
	call	write
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	cmdlen_check
	movl	%eax, -24(%rbp)
	cmpl	$0, -24(%rbp)
	jns	.L93
	movq	-32(%rbp), %rax
	movl	$1, 1176(%rax)
.L93:
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	shell_mode
	movl	%eax, -20(%rbp)
	cmpl	$0, -20(%rbp)
	je	.L91
	movl	$0, %eax
	call	error
.L91:
	movq	-48(%rbp), %rdx
	leaq	-72(%rbp), %rcx
	leaq	-64(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	getline
	movq	%rax, -40(%rbp)
	cmpq	$-1, -40(%rbp)
	jne	.L94
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	fclose
	movl	$0, %eax
.L90:
	addq	$1288, %rsp
	popq	%rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	batch_mode, .-batch_mode
.globl main
	.type	main, @function
main:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$1272, %rsp
	movl	%edi, -1268(%rbp)
	movq	%rsi, -1280(%rbp)
	leaq	-1268(%rbp), %rax
	movq	%rax, %rdi
	.cfi_offset 3, -24
	call	arg_checker
	movl	%eax, -52(%rbp)
	cmpl	$0, -52(%rbp)
	jns	.L97
	movl	$0, %eax
	call	exit_error
	jmp	.L98
.L97:
	cmpl	$0, -52(%rbp)
	jne	.L99
	jmp	.L101
.L104:
	nop
.L101:
	leaq	-1264(%rbp), %rbx
	movl	$0, %eax
	movl	$150, %edx
	movq	%rbx, %rdi
	movq	%rdx, %rcx
	rep stosq
	leaq	-1264(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	sh_prompt
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	shell_mode
	movl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	je	.L104
	movl	$0, %eax
	call	error
	jmp	.L101
.L99:
	movq	-1280(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	batch_mode
	movl	%eax, -20(%rbp)
	cmpl	$0, -20(%rbp)
	je	.L98
	movl	$0, %eax
	call	error
	movl	$1, %eax
	jmp	.L102
.L98:
	movl	$0, %eax
.L102:
	addq	$1272, %rsp
	popq	%rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-4)"
	.section	.note.GNU-stack,"",@progbits
