int main(int argc, char **argv)
{
	asm("\
		shell_begin:   \n\
		xor %rax, %rax \n\
		add $60, %rax  \n\
		xor %rdi, %rdi \n\
		add $20, %rdi  \n\
		syscall        \n\
		shell_end:     \n\
	");
	return 0;
}
