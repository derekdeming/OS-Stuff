this repository is a fun project to learn about the basics of operating systems. I also want to learn more about zig and how it compares to c so we will be converting a C based tinyOS to a zig based tinyOS.

It will cover the basics of operating systems and how they work. I will try to put as much details in the code as possible. 

I will be using the book created by Seiya Nuta called [Operating System in 1000 Lines](https://operating-system-in-1000-lines.vercel.app/en/) as a guide for the C based tinyOS. As part of the table of contents, it will cover the following topics:

- [x] 00. Introduction
- [ ] 01. Hello, World!
- [ ] 02. C Standard Library
- [ ] 03. Kernel Panic
- [ ] 04. Exceptions
- [ ] 05. Memory Management
- [ ] 06. Process Management
- [ ] 07. Page Tables
- [ ] 08. Application
- [ ] 09. User Mode and System Calls
- [ ] 10. Disk I/O
- [ ] 11. File System
- [ ] 12. Networking
- [ ] 13. Conclusion

However, I will also like to add some additional topics to the table of contents but we shall see how it goes. As of now, I have the following topics in mind but these are subject to change. I am interested in security and do security research so I am trying to add some security related topics to the table of contents.

- [ ] 14. Network Security 
- [ ] 15. Access Control and Authentication
- [ ] 16. Process Isolation and Sandboxing
- [ ] 17. Secure Boot and Trusted Execution

I am not sure if I will do the entire table of contents for the zig based tinyOS but we shall see how it goes.

We will be writing an OS for 32-bit RISC-V as the architecture for the tinyOS. It will also support [QEMU virtual machine](https://www.qemu.org/docs/master/system/riscv/virt.html)
