/*
 *
 * vnat code added by Ashwin Kashyap <cybernytrix@yahoo.com>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/ether.h>
#include <getopt.h>
#include "../include/ebtables_u.h"
#include <linux/netfilter_bridge/ebt_nat.h>

static int to_source_supplied, to_dest_supplied, to_vlan_supplied, strip_tag_supplied, vlan_qos_supplied;


#define NAT_S '1'
#define NAT_D '1'
#define NAT_V '1'
#define NAT_S_TARGET '2'
#define NAT_D_TARGET '2'
#define NAT_V_TARGET '2'
#define NAT_V_STRIP  '3'

#define NAT_V_QOS    '4'
static struct option opts_s[] =
{
        { "to-source"     , required_argument, 0, NAT_S },
        { "to-src"        , required_argument, 0, NAT_S },
        { "snat-target"   , required_argument, 0, NAT_S_TARGET },

        { 0 }
};

static struct option opts_d[] =
{
        { "to-destination", required_argument, 0, NAT_D },
        { "to-dst"        , required_argument, 0, NAT_D },
        { "dnat-target"   , required_argument, 0, NAT_D_TARGET },

        { 0 }
};

static struct option opts_v[] =
{
        { "to-vlan"       , required_argument, 0, NAT_V },
        { "vlan-qos"      , required_argument, 0, NAT_V_QOS },
        { "strip-tag"     , no_argument      , 0, NAT_V_STRIP },

        { "vnat-target"   , required_argument, 0, NAT_V_TARGET },
        { 0 }
};

static void print_help_s()
{
        printf(
        "snat options:\n"
        " --to-src address       : MAC address to map source to\n"

        " --snat-target target   : ACCEPT, DROP, RETURN or CONTINUE\n");
}

static void print_help_d()
{
        printf(
        "dnat options:\n"
        " --to-dst address       : MAC address to map destination to\n"

        " --dnat-target target   : ACCEPT, DROP, RETURN or CONTINUE\n");
}

static void print_help_v()
{
        printf(
        "vnat options:\n"
        " --to-vlan tag          : Vlan ID to be tagged with\n"

        " --strip-tag            : Gets rid of the tag if present\n"
        " --vlan-qos             : Vlan priority\n"
        " --vnat-target target   : ACCEPT, DROP, RETURN or CONTINUE\n");

}

static void init_s(struct ebt_entry_target *target)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;

        to_source_supplied = 0;
        natinfo->target = EBT_ACCEPT;

        return;
}

static void init_d(struct ebt_entry_target *target)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;

        to_dest_supplied = 0;
        natinfo->target = EBT_ACCEPT;

        return;
}

static void init_v(struct ebt_entry_target *target)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;

        to_vlan_supplied = 0;
        strip_tag_supplied = 0;

        vlan_qos_supplied = 0;
        natinfo->qos = 0;
        natinfo->target = EBT_ACCEPT;
        return;
}

#define OPT_SNAT         0x01
#define OPT_SNAT_TARGET  0x02
static int parse_s(int c, char **argv, int argc,

   const struct ebt_u_entry *entry, unsigned int *flags,
   struct ebt_entry_target **target)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)(*target)->data;
        struct ether_addr *addr;


        switch (c) {
        case NAT_S:
                check_option(flags, OPT_SNAT);
                to_source_supplied = 1;
                if (!(addr = ether_aton(optarg)))
                        print_error("Problem with specified --to-source mac");

                memcpy(natinfo->mac, addr, ETH_ALEN);
                break;
        case NAT_S_TARGET:
                check_option(flags, OPT_SNAT_TARGET);
                if (FILL_TARGET(optarg, natinfo->target))

                        print_error("Illegal --snat-target target");
                break;
        default:
                return 0;
        }
        return 1;
}

#define OPT_VNAT        0x01

#define OPT_VNAT_STRIP  0x02
#define OPT_VNAT_TARGET 0x03
#define OPT_VNAT_QOS    0x04
static int parse_v(int c, char **argv, int argc,
   const struct ebt_u_entry *entry, unsigned int *flags,
   struct ebt_entry_target **target)

{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)(*target)->data;
        uint16_t vid, qos;

        switch (c) {
        case NAT_V:
                check_option(flags, OPT_VNAT);

                check_option(flags, OPT_VNAT_STRIP);
                to_vlan_supplied = 1;
                if (sscanf(optarg, "%hu", &vid) != 1 || vid > 4094)
                        print_error("Problem with specified "

                                    "vlan id, must be between 0 and 4094");
                natinfo->vid = vid;
                break;
        case NAT_V_TARGET:
                check_option(flags, OPT_VNAT_TARGET);

                if (FILL_TARGET(optarg, natinfo->target))
                        print_error("Illegal --vnat-target target");
                break;
        case NAT_V_STRIP:
                check_option(flags, OPT_VNAT);

                check_option(flags, OPT_VNAT_STRIP);
                strip_tag_supplied = 1;
                natinfo->vid = -1;
                
        case NAT_V_QOS:
                check_option(flags, OPT_VNAT_QOS);

                vlan_qos_supplied = 1;
                if (sscanf(optarg, "%hu", &qos) != 1 || vid > 7)
                        print_error("Problem with specified "
                                    "vlan qos, must be between 0 and 7");

                natinfo->qos = qos;
                break;
        default:
                return 0;
        }
        return 1;
}

#define OPT_DNAT        0x01
#define OPT_DNAT_TARGET 0x02

static int parse_d(int c, char **argv, int argc,
   const struct ebt_u_entry *entry, unsigned int *flags,
   struct ebt_entry_target **target)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)(*target)->data;

        struct ether_addr *addr;

        switch (c) {
        case NAT_D:
                check_option(flags, OPT_DNAT);
                to_dest_supplied = 1;
                if (!(addr = ether_aton(optarg)))

                        print_error("Problem with specified "
                                    "--to-destination mac");
                memcpy(natinfo->mac, addr, ETH_ALEN);
                break;

        case NAT_D_TARGET:
                check_option(flags, OPT_DNAT_TARGET);
                if (FILL_TARGET(optarg, natinfo->target))
                        print_error("Illegal --dnat-target target");

                break;
        default:
                return 0;
        }
        return 1;
}

static void final_check_s(const struct ebt_u_entry *entry,
   const struct ebt_entry_target *target, const char *name,

   unsigned int hookmask, unsigned int time)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;

        if (BASE_CHAIN && natinfo->target == EBT_RETURN)
                print_error("--snat-target RETURN not allowed on base chain");

        CLEAR_BASE_CHAIN_BIT;
        if ((hookmask & ~(1 << NF_BR_POST_ROUTING)) || strcmp(name, "nat"))
                print_error("Wrong chain for snat");
        if (time == 0 && to_source_supplied == 0)

                print_error("No snat address supplied");
}

static void final_check_d(const struct ebt_u_entry *entry,
   const struct ebt_entry_target *target, const char *name,
   unsigned int hookmask, unsigned int time)

{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;

        if (BASE_CHAIN && natinfo->target == EBT_RETURN)
                print_error("--dnat-target RETURN not allowed on base chain");

        CLEAR_BASE_CHAIN_BIT;
        if (((hookmask & ~((1 << NF_BR_PRE_ROUTING) | (1 << NF_BR_LOCAL_OUT)))
           || strcmp(name, "nat")) &&
           ((hookmask & ~(1 << NF_BR_BROUTING)) || strcmp(name, "broute")))

                print_error("Wrong chain for dnat");
        if (time == 0 && to_dest_supplied == 0)
                print_error("No dnat address supplied");
}

static void final_check_v(const struct ebt_u_entry *entry,

   const struct ebt_entry_target *target, const char *name,
   unsigned int hookmask, unsigned int time)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;

        if (BASE_CHAIN && natinfo->target == EBT_RETURN)

                print_error("--vnat-target RETURN not allowed on base chain");
        CLEAR_BASE_CHAIN_BIT;
        if ((hookmask & ~(1 << NF_BR_PRE_ROUTING)) || strcmp(name, "nat"))

                print_error("Wrong chain for vnat");
        if (time == 0 
            && ((to_vlan_supplied == 0 
                && (strip_tag_supplied == 0 || vlan_qos_supplied == 1))

            || (to_vlan_supplied == 1 && strip_tag_supplied == 1)))
                print_error("You must specify only one of --to-vlan or --strip-tag. "
                "You cannot specify --vlan-qos with --strip-tag");

}

static void print_s(const struct ebt_u_entry *entry,
   const struct ebt_entry_target *target)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;

        printf("--to-src ");

        printf("%s", ether_ntoa((struct ether_addr *)natinfo->mac));
        printf(" --snat-target %s", TARGET_NAME(natinfo->target));
}

static void print_d(const struct ebt_u_entry *entry,

   const struct ebt_entry_target *target)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;

        printf("--to-dst ");
        printf("%s", ether_ntoa((struct ether_addr *)natinfo->mac));

        printf(" --dnat-target %s", TARGET_NAME(natinfo->target));
}

static void print_v(const struct ebt_u_entry *entry,
   const struct ebt_entry_target *target)
{
        struct ebt_nat_info *natinfo = (struct ebt_nat_info *)target->data;


        if (natinfo->vid == (uint16_t)-1) {
                // We are stripping
                printf("--strip-tag ");
        } else {
                printf("--to-vlan %hd", natinfo->vid);

                printf(" --vlan-qos %hd", natinfo->qos);
        }
        printf(" --vnat-target %s", TARGET_NAME(natinfo->target));
}

static int compare(const struct ebt_entry_target *t1,

   const struct ebt_entry_target *t2)
{
        struct ebt_nat_info *natinfo1 = (struct ebt_nat_info *)t1->data;
        struct ebt_nat_info *natinfo2 = (struct ebt_nat_info *)t2->data;


        return !memcmp(natinfo1->mac, natinfo2->mac, sizeof(natinfo1->mac)) 

                && natinfo1->target == natinfo2->target
                && natinfo1->vid == natinfo2->vid
                && natinfo1->qos == natinfo2->qos;
}

static struct ebt_u_target snat_target =

{
        EBT_SNAT_TARGET,
        sizeof(struct ebt_nat_info),
        print_help_s,
        init_s,
        parse_s,
        final_check_s,
        print_s,
        compare,
        opts_s,

};

static struct ebt_u_target dnat_target =
{
        EBT_DNAT_TARGET,
        sizeof(struct ebt_nat_info),
        print_help_d,
        init_d,
        parse_d,
        final_check_d,
        print_d,

        compare,
        opts_d
};

static struct ebt_u_target vnat_target =
{
        EBT_VNAT_TARGET,
        sizeof(struct ebt_nat_info),
        print_help_v,
        init_v,
        parse_v,

        final_check_v,
        print_v,
        compare,
        opts_v
};

static void _init(void) __attribute__ ((constructor));
static void _init(void)
{
        register_target(&snat_target);

        register_target(&dnat_target);
        register_target(&vnat_target);
}
