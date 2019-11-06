#pragma once

#include "std_header.h"
#include "pcap_header.h"

namespace mammoth{
    class MPcapUtil{
    public:
        static pcap_t* get_device(int _in_device_num = 0);
        static pcap_t* get_device(std::string path);
    };
};

pcap_t* mammoth::MPcapUtil::get_device(std::string path){
    char errbuf[100];
	pcap_t *pfile = pcap_open_offline(path.c_str(), errbuf);
	if (NULL == pfile) {
		std::cout << "Read pcap file error: " << errbuf << std::endl;
		return nullptr;
	}
	return pfile;
}

pcap_t* mammoth::MPcapUtil::get_device(int _in_device_num){
    pcap_if_t *alldevs = NULL;
    pcap_if_t *d = NULL;
    int inum = 0;
    int i = 0;
    pcap_t *adhandle;
    char errbuf[PCAP_ERRBUF_SIZE];
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        exit(1);
    }
    /* Print the list */
    for (d = alldevs; d; d = d->next) {
        printf("%d. %s", ++i, d->name);
        if (d->description)
            printf(" (%s)\n", d->description);
        else
            printf(" (No description available)\n");
    }
    if (i == 0) {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        return nullptr;
    }
    printf("The input ethernet number is : %d", i);
    inum = _in_device_num;
    if (inum < 1 || inum > i) {
        printf("\nIntrface number out of range.\n");
        pcap_freealldevs(alldevs);
        return nullptr;
    }
    for (d = alldevs, i = 0; i< inum - 1; d = d->next, i++);
    if ((adhandle = pcap_open_live(d->name,
        65536, 1, 1000, errbuf)) == NULL) {
        fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
        pcap_freealldevs(alldevs);
        return nullptr;
    }
    printf("\nlistening on %s...\n\n", d->description);
    pcap_freealldevs(alldevs);
    return adhandle;
}