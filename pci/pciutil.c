#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <pci/pci.h>
#include <stdint.h>
#include <getopt.h>
#include <linux/types.h>

/* Suppose you have pciutils library
 * Compile: gcc -o pci pci.c -Ipciutils/lib/ -lpci */

static struct option opts[] = {
        {"device", 1, NULL, 'd' },
        {"slot", 1, NULL, 's' },
        { 0, 0, NULL, '0' }
};

static void usage(char *progname, char *idfile)
{
    printf( "Usage: %s [device]   (%s)\n\n"
            "-d [<vendor>]:[<device>]\t\t\tShow selected devices\n"
            "-s [[[[<domain>]:]<bus>]:][<slot>][.[<func>]]"
                    "\tShow devices in selected slots\n\n",
            progname, idfile);
}

int main(int argc, char **argv)
{
    struct pci_access *pacc;
    struct pci_dev *dev;
    struct pci_filter filter;
    char buf[128];
    int ret = 0;
    int device_specified = 0;
    char *errmsg;
    int ch;

    pacc = pci_alloc();                               /* Get the pci_access structure */
    if(!pacc) {
        perror("Fail: pci_alloc");
        exit(1);
    }
    pci_filter_init(pacc, &filter);

    while ((ch = getopt_long(argc, argv, "o:d:s:v:", opts, NULL)) != -1) {
        switch (ch) {
        case 'd':
            /* Show only selected devices */
            if ((errmsg = pci_filter_parse_id(&filter, optarg))) {
                printf("%s\n", errmsg);
                exit(1);
            }
            device_specified++;
            break;
        case 's':
            /* Show only devices in selected slots */
            if ((errmsg = pci_filter_parse_slot(&filter, optarg))) {
                printf("%s\n", errmsg);
                exit(1);
            }
            device_specified++;
            break;
        case '?':
        default:
            usage(argv[0], pacc->id_file_name);
            exit(1);
            break;
        }
    }

    if (!device_specified) {
        printf("No device given\n");
        usage(argv[0], pacc->id_file_name);
        exit(1);
    }

    pci_init(pacc);                                   /* Initialize the PCI library */
    pci_scan_bus(pacc);                               /* Get the list of devices    */

    /* Iterate over all devices to find the single one we want */
    for (dev = pacc->devices; dev; dev = dev->next) {
        if (pci_filter_match(&filter, dev))
            break;
    }
    if (!dev) {
        perror("Fail: No device found!\n");
        ret = -1;
        goto out;
    }

    /* Show device information. Ex: name, vendor, id, ...*/
    printf( "%02x:%02x.%d (%04x:%04x)\n%s\n",
                 dev->bus, dev->dev, dev->func,
                 dev->vendor_id, dev->device_id,
                 pci_lookup_name(pacc, buf, sizeof(buf),
                                 PCI_LOOKUP_VENDOR|PCI_LOOKUP_DEVICE,
                                 dev->vendor_id, dev->device_id, 0, 0));

out:
        pci_cleanup(pacc);          /* Close everything */
        return ret;
}
