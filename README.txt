Topics In Networks
Performance of queuing in a packet switch for different scheduling algorithms.
Submitted by: Astik Raj

Commands : 
------------------------------------------------------------------------------------------------
# Environment : Windows
# Open terminal in the same directory.
# Run the following commands:
	g++ code.cpp
	./a.exe − N switchportcount −B buffersize −p packetgenprob −queue INQ| KOUQ| ISLIP −K knockout −out − outputfile −T maxtimeslots
------------------------------------------------------------------------------------------------

Purpose:
Performance of queuing in a packet switch for different scheduling algorithms.
It contains implementation of 3 scheduling algorithms:
• INQ:– For each packet generated, if there is no contention for its
	desired output port, it is selected for transmission and placed in
	the corresponding output port’s buffer. For packets contending for
	the same output port, one of the packets is randomly selected for
	transmission and placed in the corresponding output port’s buffer;
	the other packets are queued at the corresponding input port.
• KOUQ:– A maximum of K packets (per output port) that arrive in a given 
	slot are queued (based on packet arrival time) at the corresponding
	output port. If two or more packets have the same arrival time,
	the packets can be queued in any order. If more than K packets
	arrive in a slot for a particular output port, then K packets are
	randomly selected for buffering, and the remaining packets are 
	dropped.
	The default value is K = 0.6N.
• iSLIP:– Implement the iSLIP scheduling algorithm with a Virtual
	Output Queue (VOQ). The algorithm is explained in the reference 
	provided.
