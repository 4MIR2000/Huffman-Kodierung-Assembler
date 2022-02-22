.intel_syntax noprefix

.extern frequency_analysis
.extern quicksort_result
.extern dictionary
.extern coding

.global huffman_encode

// rdi char* data
// rsi char* result
// rdx unsigned int result_size

huffman_encode:
	push rdx
	push rdi
	push rsi

	call frequency_analysis	// rax Größe der Häufigkeitsanalyse in short

	cmp rax, 0
	jle .Lende1
	
	pop rdi
	push rdi
	push rax

	sar rax, 1
	dec rax
	mov rdx, rax
	xor rsi, rsi
	
	call quicksort_result

	pop rsi
	shl rsi
	pop rdi
	push rdi

	call dictionary		// rax Größe des Wörterbuchs

	cmp rax, 0
	jle .Lende1

	pop rsi
	pop rdi
	pop rdx
	push rax
	
	call coding			// rax Größe der Kodierung
	
	cmp rax, 0
	jle .Lende2
	pop r8
	add rax, r8
	inc rax

	ret

	.Lende1:
		add rsp, 24
		ret

	.Lende2:
		add rsp, 8
		ret

