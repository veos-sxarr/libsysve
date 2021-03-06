#include <stddef.h>
#include <stdlib.h>
#include <libvhcall.h>
#include <stdio.h>

#define MAXLEN 100
const char *msg = "Hello, This is VE";

int main() {
	int ret=1;
	uint64_t len = MAXLEN;
	char buf[MAXLEN];
	int64_t symid;
	vhcall_handle handle;
	vhcall_args *ca;
	uint64_t retval = -1;
	/* Load VH C library */
	printf("Test for C library\n");
	handle = vhcall_install("./libvhhello.so");
	if (handle == (vhcall_handle)-1) {
		perror("vhcall_install");
                goto ret;
        }
	/* Find VH C library function */
	symid  = vhcall_find(handle, "hello");
        if (symid == -1 ) {
                perror("vhcall_find");
                goto ret2;
        }
	/* Allocate arguments object for VH library function */
	ca  = vhcall_args_alloc();
        if (!ca) {
                perror("vhcall_args_alloc");
                goto ret2;
        }
	/* Set arguments */
	memcpy(buf, msg, strlen(msg));
	memset(buf+strlen(msg)+1, '\0', MAXLEN-strlen(msg));
	/* 1st */
	ret = vhcall_args_set_pointer(ca, VHCALL_INTENT_INOUT, 0, buf, MAXLEN);
	if (ret) {
		perror("vhcall_args_set_pointer");
		goto ret3;
	}
	/* 2nd */
	ret = vhcall_args_set_i32(ca, 1, -1);
	if (ret) {
                perror("vhcall_args_set_i32");
                goto ret3;
        }
	/* 3rd */
	ret = vhcall_args_set_float(ca, 2, -1.111);
        if (ret) {
                perror("vhcall_args_set_float");
                goto ret3;
        }
	/* Invoke VH C library function */
	printf("[VE] buffer: %s\n", buf);
	ret = vhcall_invoke_with_args(symid, ca, &retval);
	if (ret) {
                perror("vhcall_invoke_with_args");
                goto ret3;
        }
	printf("[VE] buffer: %s\n", buf);
	/* Check return value of function */
	if (retval) {
		printf("VH function returns unexpected value(%ld)\n", retval);
                goto ret3;
        }
ret3:
	/* Free arguments object */
	vhcall_args_free(ca);
ret2:
	/* Unload VH library */
	if (vhcall_uninstall(handle))
                perror("vhcall_uninstall");
ret:
	return ret;
}
