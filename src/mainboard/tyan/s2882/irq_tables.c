#include <console/console.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>

static unsigned node_link_to_bus(unsigned node, unsigned link)
{
        device_t dev;
        unsigned reg;

        dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));
        if (!dev) {
                return 0;
        }
        for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
                uint32_t config_map;
                unsigned dst_node;
                unsigned dst_link;
                unsigned bus_base;
                config_map = pci_read_config32(dev, reg);
                if ((config_map & 3) != 3) {
                        continue;
                }
                dst_node = (config_map >> 4) & 7;
                dst_link = (config_map >> 8) & 3;
                bus_base = (config_map >> 16) & 0xff;
                if ((dst_node == node) && (dst_link == link))
                {
                        return bus_base;
                }
        }
        return 0;
}

static void write_pirq_info(struct irq_info *pirq_info, uint8_t bus, uint8_t devfn, uint8_t link0, uint16_t bitmap0,
		uint8_t link1, uint16_t bitmap1, uint8_t link2, uint16_t bitmap2,uint8_t link3, uint16_t bitmap3,
		uint8_t slot, uint8_t rfu)
{
        pirq_info->bus = bus;
        pirq_info->devfn = devfn;
                pirq_info->irq[0].link = link0;
                pirq_info->irq[0].bitmap = bitmap0;
                pirq_info->irq[1].link = link1;
                pirq_info->irq[1].bitmap = bitmap1;
                pirq_info->irq[2].link = link2;
                pirq_info->irq[2].bitmap = bitmap2;
                pirq_info->irq[3].link = link3;
                pirq_info->irq[3].bitmap = bitmap3;
        pirq_info->slot = slot;
        pirq_info->rfu = rfu;
}

unsigned long write_pirq_routing_table(unsigned long addr)
{

	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	unsigned slot_num;
	uint8_t *v;

        uint8_t sum=0;
        int i;

        unsigned char bus_chain_0;
        unsigned char bus_8131_1;
        unsigned char bus_8131_2;
        unsigned char bus_8111_1;
        {
                device_t dev;

                /* HT chain 0 */
                bus_chain_0 = node_link_to_bus(0, 0);
                if (bus_chain_0 == 0) {
                        printk(BIOS_DEBUG, "ERROR - cound not find bus for node 0 chain 0, using defaults\n");
                        bus_chain_0 = 1;
                }

                /* 8111 */
                dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x03,0));
                if (dev) {
                        bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                }
                else {
                        printk(BIOS_DEBUG, "ERROR - could not find PCI 1:03.0, using defaults\n");

                        bus_8111_1 = 4;
                }
                /* 8131-1 */
                dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x01,0));
                if (dev) {
                        bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);

                }
                else {
                        printk(BIOS_DEBUG, "ERROR - could not find PCI 1:01.0, using defaults\n");

                        bus_8131_1 = 2;
                }
                /* 8131-2 */
                dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x02,0));
                if (dev) {
                        bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);

                }
                else {
                        printk(BIOS_DEBUG, "ERROR - could not find PCI 1:02.0, using defaults\n");

                        bus_8131_2 = 3;
                }
        }

        /* Align the table to be 16 byte aligned. */
        addr += 15;
        addr &= ~15;

        /* This table must be between 0xf0000 & 0x100000 */
        printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...\n", addr);

	pirq = (void *)(addr);
	v = (uint8_t *)(addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version  = PIRQ_VERSION;

	pirq->rtr_bus = bus_chain_0;
	pirq->rtr_devfn = (4<<3)|3;

	pirq->exclusive_irqs = 0;

	pirq->rtr_vendor = 0x1022;
	pirq->rtr_device = 0x746b;

	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *) ( &pirq->checksum + 1);
	slot_num = 0;

        {
                device_t dev;
                dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x04,3));
                if (dev) {
                        /* initialize PCI interupts - these assignments depend
                        on the PCB routing of PINTA-D

                        PINTA = IRQ5
                        PINTB = IRQ9
                        PINTC = IRQ11
                        PINTD = IRQ10
                        */
                        pci_write_config16(dev, 0x56, 0xab95);
                }
        }

        printk(BIOS_DEBUG, "setting Onboard AMD Southbridge\n");
        static const unsigned char slotIrqs_1_4[4] = { 5, 9, 11, 10 };
        pci_assign_irqs(bus_chain_0, 4, slotIrqs_1_4);
        write_pirq_info(pirq_info, bus_chain_0,(4<<3)|0, 0x1, 0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Onboard AMD USB\n");
        static const unsigned char slotIrqs_8111_1_0[4] = { 0, 0, 0, 10 };
        pci_assign_irqs(bus_8111_1, 0, slotIrqs_8111_1_0);
        write_pirq_info(pirq_info, bus_8111_1,0, 0, 0, 0, 0, 0, 0, 0x4, 0xdef8, 0, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Onboard ATI Display Adapter\n");
        static const unsigned char slotIrqs_8111_1_6[4] = { 11, 0, 0, 0 };
        pci_assign_irqs(bus_8111_1, 6, slotIrqs_8111_1_6);
        write_pirq_info(pirq_info, bus_8111_1,(6<<3)|0, 0x3, 0xdef8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Slot 1\n");
        static const unsigned char slotIrqs_8131_2_3[4] = { 5, 9, 11, 10 };
        pci_assign_irqs(bus_8131_2, 3, slotIrqs_8131_2_3);
        write_pirq_info(pirq_info, bus_8131_2,(3<<3)|0, 0x1, 0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0x1, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Slot 2\n");
        static const unsigned char slotIrqs_8131_2_1[4] = { 9, 11, 10, 5 };
        pci_assign_irqs(bus_8131_2, 1, slotIrqs_8131_2_1);
        write_pirq_info(pirq_info, bus_8131_2,(1<<3)|0, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0x1, 0xdef8, 0x2, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Slot 3\n");
        static const unsigned char slotIrqs_8131_1_3[4] = { 10, 5, 9, 11 };
        pci_assign_irqs(bus_8131_1, 3, slotIrqs_8131_1_3);
        write_pirq_info(pirq_info, bus_8131_1,(3<<3)|0, 0x4, 0xdef8, 0x1, 0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x3, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Slot 4\n");
        static const unsigned char slotIrqs_8131_1_2[4] = { 11, 10, 5, 9 };
        pci_assign_irqs(bus_8131_1, 2, slotIrqs_8131_1_2);
        write_pirq_info(pirq_info, bus_8131_1,(2<<3)|0, 0x3, 0xdef8, 0x4, 0xdef8, 0x1, 0xdef8, 0x2, 0xdef8, 0x4, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Slot 5\n");
        static const unsigned char slotIrqs_8111_1_4[4] = { 5, 9, 11, 10 };
        pci_assign_irqs(bus_8111_1, 4, slotIrqs_8111_1_4);
        write_pirq_info(pirq_info, bus_8111_1,(4<<3)|0, 0x1, 0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0x5, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Onboard SI Serial ATA\n");
        static const unsigned char slotIrqs_8111_1_5[4] = { 10, 0, 0, 0 };
        pci_assign_irqs(bus_8111_1, 5, slotIrqs_8111_1_5);
        write_pirq_info(pirq_info, bus_8111_1,(5<<3)|0, 0x4, 0xdef8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Onboard Intel NIC\n");
        static const unsigned char slotIrqs_8111_1_8[4] = { 11, 0, 0, 0 };
        pci_assign_irqs(bus_8111_1, 8, slotIrqs_8111_1_8);
        write_pirq_info(pirq_info, bus_8111_1,(8<<3)|0, 0x3, 0xdef8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Onboard Adaptec  SCSI\n");
        static const unsigned char slotIrqs_8131_1_6[4] = { 5, 9, 0, 0 };
        pci_assign_irqs(bus_8131_1, 6, slotIrqs_8131_1_6);
        write_pirq_info(pirq_info, bus_8131_1,(6<<3)|0, 0x1, 0xdef8, 0x2, 0xdef8, 0, 0, 0, 0, 0, 0);
	pirq_info++; slot_num++;

        printk(BIOS_DEBUG, "setting Onboard Broadcom NIC\n");
        static const unsigned char slotIrqs_8131_1_9[4] = { 5, 9, 0, 0 };
        pci_assign_irqs(bus_8131_1, 9, slotIrqs_8131_1_9);
        write_pirq_info(pirq_info, bus_8131_1,(9<<3)|0, 0x1, 0xdef8, 0x2, 0xdef8, 0, 0, 0, 0, 0, 0);
	pirq_info++; slot_num++;

	pirq->size = 32 + 16 * slot_num;

        for (i = 0; i < pirq->size; i++)
                sum += v[i];

	sum = pirq->checksum - sum;

        if (sum != pirq->checksum) {
                pirq->checksum = sum;
        }

	printk(BIOS_INFO, "done.\n");

	return	(unsigned long) pirq_info;
}
