	.file	"fastsort.c"
	.section	.rodata
	.align 4
.LC0:
	.string	"Usage: %s -i inputfile -o outputfile\n"
	.text
.globl usage
	.type	usage, @function
usage:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__xpg_basename
	movl	%eax, -12(%ebp)
	movl	$.LC0, %edx
	movl	stderr, %eax
	movl	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
	movl	$1, (%esp)
	call	exit
	.size	usage, .-usage
.globl cmpkeys
	.type	cmpkeys, @function
cmpkeys:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$208, %esp
	movl	8(%ebp), %eax
	leal	-112(%ebp), %edx
	movl	%eax, %ebx
	movl	$25, %eax
	movl	%edx, %edi
	movl	%ebx, %esi
	movl	%eax, %ecx
	rep movsl
	movl	12(%ebp), %eax
	leal	-212(%ebp), %edx
	movl	%eax, %ebx
	movl	$25, %eax
	movl	%edx, %edi
	movl	%ebx, %esi
	movl	%eax, %ecx
	rep movsl
	movl	-112(%ebp), %edx
	movl	-212(%ebp), %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	addl	$208, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.size	cmpkeys, .-cmpkeys
	.section	.rodata
.LC1:
	.string	"Number of args:%d\n"
.LC2:
	.string	""
.LC3:
	.string	"i:o:?"
.LC4:
	.string	"Error: Cannot open file %s\n"
	.align 4
.LC5:
	.string	"Error: Cannot read the contents of file %s\n"
	.align 4
.LC6:
	.string	"Error: Cannot allocate memory for %lu records -> malloc failed\n"
.LC7:
	.string	"Error: Cannot read file %s\n"
	.align 4
.LC8:
	.string	"Number of records sorted: %lu\n"
	.align 4
.LC9:
	.string	"Error: Cannot write to file %s\n"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$292, %esp
	movl	$.LC1, %eax
	movl	8(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	cmpl	$5, 8(%ebp)
	je	.L6
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	usage
.L6:
	movl	$.LC2, 228(%esp)
	movl	$.LC2, 232(%esp)
	movl	$0, opterr
	jmp	.L7
.L12:
	movl	236(%esp), %eax
	cmpl	$105, %eax
	je	.L10
	cmpl	$111, %eax
	je	.L11
	cmpl	$63, %eax
	je	.L9
	jmp	.L8
.L10:
	movl	optarg, %eax
	movl	%eax, (%esp)
	call	strdup
	movl	%eax, 228(%esp)
	jmp	.L7
.L11:
	movl	optarg, %eax
	movl	%eax, (%esp)
	call	strdup
	movl	%eax, 232(%esp)
	jmp	.L7
.L9:
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	usage
.L8:
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	usage
.L7:
	movl	$.LC3, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	getopt
	movl	%eax, 236(%esp)
	cmpl	$-1, 236(%esp)
	jne	.L12
	movl	optind, %eax
	sall	$2, %eax
	addl	12(%ebp), %eax
	movl	(%eax), %eax
	testl	%eax, %eax
	je	.L13
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	usage
.L13:
	movl	$0, 4(%esp)
	movl	228(%esp), %eax
	movl	%eax, (%esp)
	call	open
	movl	%eax, 240(%esp)
	cmpl	$0, 240(%esp)
	jns	.L14
	movl	$.LC4, %edx
	movl	stderr, %eax
	movl	228(%esp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
	movl	$1, (%esp)
	call	exit
.L14:
	movl	$448, 8(%esp)
	movl	$577, 4(%esp)
	movl	232(%esp), %eax
	movl	%eax, (%esp)
	call	open
	movl	%eax, 244(%esp)
	cmpl	$0, 244(%esp)
	jns	.L15
	movl	$.LC4, %edx
	movl	stderr, %eax
	movl	232(%esp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
	movl	$1, (%esp)
	call	exit
.L15:
	leal	140(%esp), %eax
	movl	%eax, 248(%esp)
	movl	248(%esp), %eax
	movl	%eax, 4(%esp)
	movl	240(%esp), %eax
	movl	%eax, (%esp)
	call	fstat
	movl	%eax, 252(%esp)
	cmpl	$0, 252(%esp)
	jns	.L16
	movl	$.LC5, %edx
	movl	stderr, %eax
	movl	228(%esp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
	movl	$1, (%esp)
	call	exit
.L16:
	movl	248(%esp), %eax
	movl	44(%eax), %eax
	movl	%eax, 256(%esp)
	movl	256(%esp), %eax
	movl	%eax, 28(%esp)
	movl	$1374389535, %edx
	movl	28(%esp), %eax
	mull	%edx
	movl	%edx, %eax
	shrl	$5, %eax
	movl	%eax, 260(%esp)
	movl	260(%esp), %eax
	imull	$100, %eax, %eax
	movl	%eax, (%esp)
	call	malloc
	movl	%eax, 264(%esp)
	cmpl	$0, 264(%esp)
	jne	.L17
	movl	$.LC6, %edx
	movl	stderr, %eax
	movl	260(%esp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
	movl	$1, (%esp)
	call	exit
.L17:
	leal	40(%esp), %eax
	movl	%eax, 268(%esp)
	movl	$0, 272(%esp)
	jmp	.L22
.L29:
	nop
.L22:
	movl	$100, 8(%esp)
	movl	268(%esp), %eax
	movl	%eax, 4(%esp)
	movl	240(%esp), %eax
	movl	%eax, (%esp)
	call	read
	movl	%eax, 280(%esp)
	cmpl	$0, 280(%esp)
	jns	.L18
	movl	$.LC7, %edx
	movl	stderr, %eax
	movl	228(%esp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
	movl	$1, (%esp)
	call	exit
.L18:
	cmpl	$0, 280(%esp)
	je	.L28
.L19:
	movl	272(%esp), %eax
	cmpl	260(%esp), %eax
	je	.L29
	movl	272(%esp), %eax
	imull	$100, %eax, %eax
	movl	%eax, %edx
	addl	264(%esp), %edx
	movl	268(%esp), %eax
	movl	%eax, %ebx
	movl	$25, %eax
	movl	%edx, %edi
	movl	%ebx, %esi
	movl	%eax, %ecx
	rep movsl
	addl	$1, 272(%esp)
	jmp	.L22
.L28:
	nop
.L27:
	movl	240(%esp), %eax
	movl	%eax, (%esp)
	call	close
	movl	$cmpkeys, 12(%esp)
	movl	$100, 8(%esp)
	movl	260(%esp), %eax
	movl	%eax, 4(%esp)
	movl	264(%esp), %eax
	movl	%eax, (%esp)
	call	qsort
	movl	$.LC8, %eax
	movl	260(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	movl	$0, 276(%esp)
	movl	$0, 276(%esp)
	jmp	.L23
.L25:
	movl	276(%esp), %eax
	imull	$100, %eax, %eax
	addl	264(%esp), %eax
	movl	$100, 8(%esp)
	movl	%eax, 4(%esp)
	movl	244(%esp), %eax
	movl	%eax, (%esp)
	call	write
	movl	%eax, 284(%esp)
	cmpl	$100, 284(%esp)
	je	.L24
	movl	$.LC9, %edx
	movl	stderr, %eax
	movl	232(%esp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
	movl	$1, (%esp)
	call	exit
.L24:
	addl	$1, 276(%esp)
.L23:
	movl	276(%esp), %eax
	cmpl	260(%esp), %eax
	jb	.L25
	movl	244(%esp), %eax
	movl	%eax, (%esp)
	call	close
	movl	264(%esp), %eax
	movl	%eax, (%esp)
	call	free
	movl	$0, %eax
	addl	$292, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	movl	%ebp, %esp
	popl	%ebp
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-4)"
	.section	.note.GNU-stack,"",@progbits
