#
# $File: ctl_eth.s
# $Date: Tue Nov 26 09:53:38 2013 +0800
# $Author: Liang Dun
#

# Assembly implementation of memtrans.cc

# t0: m_com_data = (hw_ptr_t)0x9FD003F8;
# t1: m_com_stat = (hw_ptr_t)0x9FD003FC;
# t2: m_segdisp = (hw_ptr_t)0x9FD00400;
# t3: m_flash_start = (hw_ptr_t)0x9E000000;
# t4: m_ram_start = (hw_ptr_t)0x80000000;
# t9: m_checksum

# subroutine calling convention:
#	param: $a0 or none, param may be modified
#	retval: $v0
# $a1~$a3 would not be modified
# $t5~8 may be modified

#define COM_DATA	t0
#define COM_STAT	t1
#define CHECKSUM	t9
#define SEGDISP		t2
#define FLASH_START	t3
#define RAM_START	t4
#define ETH_ADDR    t3

main:
	jal init
inf_loop:
    li $a0, 0b10100101
    sw $a0, 0($SEGDISP)

	main_loop:

        jal read_com_byte
        move $a1, $v0 # a1: cmd
        sw $a1, 0($SEGDISP)
        move $t4, $v0

        jal read_com_byte
        move $a1, $v0 # a1: cmd
        sw $a1, 0($SEGDISP)
        move $t5, $v0
        
        jal read_com_byte
        move $a1, $v0 # a1: cmd
        sw $a1, 0($SEGDISP)
        move $t6, $v0

        jal read_com_byte

        li $t7, 0b1
        beq $t6, $t7, read_eth
        sw $t4, 0($ETH_ADDR)
        sw $t5, 4($ETH_ADDR)
        b main_loop

        read_eth:
        sw $t4, 0($ETH_ADDR)
        sw $t4, 0($ETH_ADDR)
        sw $t4, 0($ETH_ADDR)
        sw $t4, 0($ETH_ADDR)
        lw $t5, 4($ETH_ADDR)
        lw $t5, 4($ETH_ADDR)
        lw $t5, 4($ETH_ADDR)
        lw $t5, 4($ETH_ADDR)
        sw $t5, 0($SEGDISP)
        b main_loop
        

init:
	li $COM_DATA, 0x9FD003F8
	li $COM_STAT, 0x9FD003FC 
	li $SEGDISP, 0x9FD00400
	li $ETH_ADDR, 0x9FD003E0
	li $RAM_START, 0x80000000 # m_ram_start

	jr $ra

read_com_byte:
	# XXX: DIRTY HACK
	# use v1 and sp for register backup
	move $v1, $a0
	move $sp, $a1

	# while (!(*m_com_stat & COM_STAT_READ));
	read_com_byte_wait_loop:
		lw $a0, 0($COM_STAT)
		andi $a0, 0x2
		beqz $a0, read_com_byte_wait_loop

	# uint8_t rst = *m_com_data;
	lw $v0, 0($COM_DATA)
	# m_checksum ^= rst;
	xor $CHECKSUM, $CHECKSUM, $v0
	# return rst;

	move $a0, $v1
	move $a1, $sp
	jr $ra


# param: $a0: size
read_com_word:
	move $t8, $ra

	li $t7, 0 # temporal retval
	li $t6, 0 # i * 8
	sll $a0, 3
	read_com_word_read_byte_loop:
		#for (int i = 0; i < size; i ++)
		#	rst |= ((uint32_t)read_com_byte()) << (i * 8);
		#return rst;
		jal read_com_byte
		sllv $v0, $v0, $t6 
		or $t7, $v0  
		addiu $t6, 8
		bne $t6, $a0, read_com_word_read_byte_loop

	move $ra, $t8
	move $v0, $t7
	jr $ra


# param: $a0, byte to write, only lowest 8 bit is effective
write_com_byte:
	# while (!(*m_com_stat & COM_STAT_WRITE));
	write_com_byte_wait_loop:
		lw $t5, 0($COM_STAT)
		andi $t5, 0x1
		beqz $t5, write_com_byte_wait_loop

	sw $a0, 0($COM_DATA)
	xor $CHECKSUM, $CHECKSUM, $a0

	jr $ra

# vim: ft=mips
