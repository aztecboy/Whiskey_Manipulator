
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
//STRUCTS
struct FromAddress{
	uint8_t error_code;
	uint8_t data;
	uint32_t data32;

};
//END OF STRUCTS


/*
Checks if the process, pointed to by the first parameter which should be the id of the process exists. returns 0 if false, returns 1 if true.
*/
char check_process_exists(int pid){
	kill(pid,0);
	switch(errno){
		case ESRCH:
			return 0;
		default:
			return 1;
	}
	return 0;
}

/*
attaches to the process, the process is pointed to by the first parameter which should be the id of the process.
returns 1 if it failed to attach 2 if it failed because of invalid perms or 3 it failed because no such process exists, returns 1 if it successfully attached.
*/

char attach_process(int pid){
	ptrace(PTRACE_ATTACH,pid,0,0);

	waitpid(pid);

	ptrace(PTRACE_CONT,pid);

	switch(errno){

		case EPERM:
			return 2;

		case ESRCH:
			return 3;

		default:
			return 0;
	}
	
	return 1;
	

}
/*
outputs the ptrace attach log into stdout, may remove this in later versions
*/
void attach_error(){

	perror("attach");
}


/*
detaches a process, the process should go into the pid parameter.
*/
char dettach_process(int pid){

	ptrace(PTRACE_DETACH,pid,0,0);

	return 0;

}


/*
outputs the ptrace attach log into stdout, may remove this in later versions
*/
void dettach_error(){

	perror("dettach");
}
/* 
reads an address and returns the byte from the address, returns error_code 1 in the struct if the address doesnt exist/wasnt allocated
*/
struct FromAddress read_address8(int pid,int* address){
	struct FromAddress from_address;

	from_address.data=ptrace(PTRACE_PEEKDATA,pid,address,0);

	switch(errno){
		case EFAULT:
			from_address.error_code=1;
			break;

		case EIO:
			from_address.error_code=1;
			break;

		default:
			from_address.error_code=0;
			break;		
	}

	return from_address;
}
/* 
reads an address and returns the long from the address, returns error_code 1 in the struct if the address doesnt exist/wasnt allocated. is little endain
*/
struct FromAddress read_address32little(int pid,int* address){

	struct FromAddress from_address={0};

	unsigned char data_array[3]={0};

	for(int buffer=0;buffer<=3;buffer++){

		data_array[buffer]=ptrace(PTRACE_PEEKDATA,pid,address+buffer,0);

		printf("<%d\n",data_array[buffer]);

		switch(errno){
			case EFAULT:
				from_address.error_code=1;
				return from_address;
				break;

			case EIO:
				from_address.error_code=1;
				return from_address;
				break;

			default:
				from_address.error_code=0;
				break;
		}
		
	}
	
	
	from_address.data32=(data_array[3] << 24) | (data_array[2] << 16) | data_array[1] | data_array[0];

	//from_address.data32=(data_array[0] << 24) | (data_array[1] << 16) | (data_array[2] << 8) | data_array[3];
	
	printf("C %d\n",from_address.data32);

	return from_address;
}
/*
outputs the logs from read_from_address into stdout, may be removed in later versions
*/
void read_error(){

	perror("peekdata");

}

/*
Writes the data in the second parameter to the address contained in the third parameter
*/
char write_address8(int pid,int* address,unsigned char data){

	ptrace(PTRACE_POKEDATA,pid,address,data);

	switch(errno){
		case EFAULT:
			return 1;

		case EIO:
			return 1;

		default:
			return 0;

	}

	return 1;


}

/*
Writes the data in the second parameter to the address contained in the third parameter
*/
char write_address32little(int pid,int* address,unsigned long data){
	unsigned char data_array[3];

	data_array[0] = data & 0xFF;

    data_array[1] = (data >> 8) & 0xFF;

    data_array[2] = (data >> 16) & 0xFF;

    data_array[3] = (data >> 24) & 0xFF;

	for(int buffer=0;buffer<3;buffer++){
		ptrace(PTRACE_POKEDATA,pid,address+buffer,data_array[buffer]);

		switch(errno){

			case EFAULT:
				return 1;
				break;

			case EIO:
				return 1;
				break;

			default:
				break;		
		}
	}
	return 0;
}
