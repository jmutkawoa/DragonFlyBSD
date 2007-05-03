# DragonFly system call names.
# DO NOT EDIT-- this file is automatically generated.
# $DragonFly: src/sys/sys/syscall.mk,v 1.57 2007/05/03 23:04:36 dillon Exp $
# created from DragonFly: src/sys/kern/syscalls.master,v 1.54 2007/04/22 00:59:25 dillon Exp 
MIASM =  \
	syscall.o \
	exit.o \
	fork.o \
	read.o \
	write.o \
	open.o \
	close.o \
	wait4.o \
	link.o \
	unlink.o \
	chdir.o \
	fchdir.o \
	mknod.o \
	chmod.o \
	chown.o \
	break.o \
	getfsstat.o \
	getpid.o \
	mount.o \
	unmount.o \
	setuid.o \
	getuid.o \
	geteuid.o \
	ptrace.o \
	recvmsg.o \
	sendmsg.o \
	recvfrom.o \
	accept.o \
	getpeername.o \
	getsockname.o \
	access.o \
	chflags.o \
	fchflags.o \
	sync.o \
	kill.o \
	getppid.o \
	dup.o \
	pipe.o \
	getegid.o \
	profil.o \
	ktrace.o \
	getgid.o \
	getlogin.o \
	setlogin.o \
	acct.o \
	sigaltstack.o \
	ioctl.o \
	reboot.o \
	revoke.o \
	symlink.o \
	readlink.o \
	execve.o \
	umask.o \
	chroot.o \
	msync.o \
	vfork.o \
	sbrk.o \
	sstk.o \
	vadvise.o \
	munmap.o \
	mprotect.o \
	madvise.o \
	mincore.o \
	getgroups.o \
	setgroups.o \
	getpgrp.o \
	setpgid.o \
	setitimer.o \
	swapon.o \
	getitimer.o \
	getdtablesize.o \
	dup2.o \
	fcntl.o \
	select.o \
	fsync.o \
	setpriority.o \
	socket.o \
	connect.o \
	getpriority.o \
	bind.o \
	setsockopt.o \
	listen.o \
	gettimeofday.o \
	getrusage.o \
	getsockopt.o \
	readv.o \
	writev.o \
	settimeofday.o \
	fchown.o \
	fchmod.o \
	setreuid.o \
	setregid.o \
	rename.o \
	flock.o \
	mkfifo.o \
	sendto.o \
	shutdown.o \
	socketpair.o \
	mkdir.o \
	rmdir.o \
	utimes.o \
	adjtime.o \
	setsid.o \
	quotactl.o \
	nfssvc.o \
	statfs.o \
	fstatfs.o \
	getfh.o \
	getdomainname.o \
	setdomainname.o \
	uname.o \
	sysarch.o \
	rtprio.o \
	semsys.o \
	msgsys.o \
	shmsys.o \
	extpread.o \
	extpwrite.o \
	ntp_adjtime.o \
	setgid.o \
	setegid.o \
	seteuid.o \
	pathconf.o \
	fpathconf.o \
	getrlimit.o \
	setrlimit.o \
	mmap.o \
	__syscall.o \
	lseek.o \
	truncate.o \
	ftruncate.o \
	__sysctl.o \
	mlock.o \
	munlock.o \
	undelete.o \
	futimes.o \
	getpgid.o \
	poll.o \
	__semctl.o \
	semget.o \
	semop.o \
	msgctl.o \
	msgget.o \
	msgsnd.o \
	msgrcv.o \
	shmat.o \
	shmctl.o \
	shmdt.o \
	shmget.o \
	clock_gettime.o \
	clock_settime.o \
	clock_getres.o \
	nanosleep.o \
	minherit.o \
	rfork.o \
	openbsd_poll.o \
	issetugid.o \
	lchown.o \
	lchmod.o \
	netbsd_lchown.o \
	lutimes.o \
	netbsd_msync.o \
	extpreadv.o \
	extpwritev.o \
	fhstatfs.o \
	fhopen.o \
	modnext.o \
	modstat.o \
	modfnext.o \
	modfind.o \
	kldload.o \
	kldunload.o \
	kldfind.o \
	kldnext.o \
	kldstat.o \
	kldfirstmod.o \
	getsid.o \
	setresuid.o \
	setresgid.o \
	aio_return.o \
	aio_suspend.o \
	aio_cancel.o \
	aio_error.o \
	aio_read.o \
	aio_write.o \
	lio_listio.o \
	yield.o \
	thr_sleep.o \
	thr_wakeup.o \
	mlockall.o \
	munlockall.o \
	__getcwd.o \
	sched_setparam.o \
	sched_getparam.o \
	sched_setscheduler.o \
	sched_getscheduler.o \
	sched_yield.o \
	sched_get_priority_max.o \
	sched_get_priority_min.o \
	sched_rr_get_interval.o \
	utrace.o \
	kldsym.o \
	jail.o \
	sigprocmask.o \
	sigsuspend.o \
	sigaction.o \
	sigpending.o \
	sigreturn.o \
	sigtimedwait.o \
	sigwaitinfo.o \
	__acl_get_file.o \
	__acl_set_file.o \
	__acl_get_fd.o \
	__acl_set_fd.o \
	__acl_delete_file.o \
	__acl_delete_fd.o \
	__acl_aclcheck_file.o \
	__acl_aclcheck_fd.o \
	extattrctl.o \
	extattr_set_file.o \
	extattr_get_file.o \
	extattr_delete_file.o \
	aio_waitcomplete.o \
	getresuid.o \
	getresgid.o \
	kqueue.o \
	kevent.o \
	sctp_peeloff.o \
	sendfile.o \
	varsym_set.o \
	varsym_get.o \
	varsym_list.o \
	upc_register.o \
	upc_control.o \
	caps_sys_service.o \
	caps_sys_client.o \
	caps_sys_close.o \
	caps_sys_put.o \
	caps_sys_reply.o \
	caps_sys_get.o \
	caps_sys_wait.o \
	caps_sys_abort.o \
	caps_sys_getgen.o \
	caps_sys_setgen.o \
	exec_sys_register.o \
	exec_sys_unregister.o \
	sys_checkpoint.o \
	mountctl.o \
	umtx_sleep.o \
	umtx_wakeup.o \
	jail_attach.o \
	set_tls_area.o \
	get_tls_area.o \
	closefrom.o \
	stat.o \
	fstat.o \
	lstat.o \
	fhstat.o \
	getdirentries.o \
	getdents.o \
	usched_set.o \
	extaccept.o \
	extconnect.o \
	syslink.o \
	mcontrol.o \
	vmspace_create.o \
	vmspace_destroy.o \
	vmspace_ctl.o \
	vmspace_mmap.o \
	vmspace_munmap.o \
	vmspace_mcontrol.o \
	vmspace_pread.o \
	vmspace_pwrite.o \
	extexit.o \
	lwp_create.o \
	lwp_gettid.o \
	lwp_kill.o \
	lwp_rtprio.o
