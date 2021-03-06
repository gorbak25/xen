/******************************************************************************
 * xen/asm-x86/guest_pt.h
 *
 * Types and accessors for guest pagetable entries, as distinct from
 * Xen's pagetable types. 
 *
 * Users must #define GUEST_PAGING_LEVELS to 2, 3 or 4 before including
 * this file.
 *
 * Parts of this code are Copyright (c) 2006 by XenSource Inc.
 * Parts of this code are Copyright (c) 2006 by Michael A Fetterman
 * Parts based on earlier work by Michael A Fetterman, Ian Pratt et al.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _XEN_ASM_GUEST_PT_H
#define _XEN_ASM_GUEST_PT_H

#if !defined(GUEST_PAGING_LEVELS)
#error GUEST_PAGING_LEVELS not defined
#endif

#define VALID_GFN(m) (m != gfn_x(INVALID_GFN))

static inline paddr_t
gfn_to_paddr(gfn_t gfn)
{
    return ((paddr_t)gfn_x(gfn)) << PAGE_SHIFT;
}

/* Override get_gfn to work with gfn_t */
#undef get_gfn
#define get_gfn(d, g, t) get_gfn_type((d), gfn_x(g), (t), P2M_ALLOC)


/* Types of the guest's page tables and access functions for them */

#if GUEST_PAGING_LEVELS == 2

#define GUEST_L1_PAGETABLE_ENTRIES     1024
#define GUEST_L2_PAGETABLE_ENTRIES     1024
#define GUEST_L1_PAGETABLE_SHIFT         12
#define GUEST_L2_PAGETABLE_SHIFT         22

typedef uint32_t guest_intpte_t;
typedef struct { guest_intpte_t l1; } guest_l1e_t;
typedef struct { guest_intpte_t l2; } guest_l2e_t;

#define PRI_gpte "08x"

static inline gfn_t guest_l1e_get_gfn(guest_l1e_t gl1e)
{ return _gfn(gl1e.l1 >> PAGE_SHIFT); }
static inline gfn_t guest_l2e_get_gfn(guest_l2e_t gl2e)
{ return _gfn(gl2e.l2 >> PAGE_SHIFT); }

static inline u32 guest_l1e_get_flags(guest_l1e_t gl1e)
{ return gl1e.l1 & 0xfff; }
static inline u32 guest_l2e_get_flags(guest_l2e_t gl2e)
{ return gl2e.l2 & 0xfff; }

static inline u32 guest_l1e_get_pkey(guest_l1e_t gl1e)
{ return 0; }
static inline u32 guest_l2e_get_pkey(guest_l2e_t gl2e)
{ return 0; }

static inline guest_l1e_t guest_l1e_from_gfn(gfn_t gfn, u32 flags)
{ return (guest_l1e_t) { (gfn_x(gfn) << PAGE_SHIFT) | flags }; }
static inline guest_l2e_t guest_l2e_from_gfn(gfn_t gfn, u32 flags)
{ return (guest_l2e_t) { (gfn_x(gfn) << PAGE_SHIFT) | flags }; }

#define guest_l1_table_offset(_va)                                           \
    (((_va) >> GUEST_L1_PAGETABLE_SHIFT) & (GUEST_L1_PAGETABLE_ENTRIES - 1))
#define guest_l2_table_offset(_va)                                           \
    (((_va) >> GUEST_L2_PAGETABLE_SHIFT) & (GUEST_L2_PAGETABLE_ENTRIES - 1))

#else /* GUEST_PAGING_LEVELS != 2 */

#if GUEST_PAGING_LEVELS == 3
#define GUEST_L1_PAGETABLE_ENTRIES      512
#define GUEST_L2_PAGETABLE_ENTRIES      512
#define GUEST_L3_PAGETABLE_ENTRIES        4
#define GUEST_L1_PAGETABLE_SHIFT         12
#define GUEST_L2_PAGETABLE_SHIFT         21
#define GUEST_L3_PAGETABLE_SHIFT         30
#else /* GUEST_PAGING_LEVELS == 4 */
#define GUEST_L1_PAGETABLE_ENTRIES      512
#define GUEST_L2_PAGETABLE_ENTRIES      512
#define GUEST_L3_PAGETABLE_ENTRIES      512
#define GUEST_L4_PAGETABLE_ENTRIES      512
#define GUEST_L1_PAGETABLE_SHIFT         12
#define GUEST_L2_PAGETABLE_SHIFT         21
#define GUEST_L3_PAGETABLE_SHIFT         30
#define GUEST_L4_PAGETABLE_SHIFT         39
#endif

typedef l1_pgentry_t guest_l1e_t;
typedef l2_pgentry_t guest_l2e_t;
typedef l3_pgentry_t guest_l3e_t;
#if GUEST_PAGING_LEVELS >= 4
typedef l4_pgentry_t guest_l4e_t;
#endif
typedef intpte_t guest_intpte_t;

#define PRI_gpte "016"PRIx64

static inline gfn_t guest_l1e_get_gfn(guest_l1e_t gl1e)
{ return _gfn(l1e_get_paddr(gl1e) >> PAGE_SHIFT); }
static inline gfn_t guest_l2e_get_gfn(guest_l2e_t gl2e)
{ return _gfn(l2e_get_paddr(gl2e) >> PAGE_SHIFT); }
static inline gfn_t guest_l3e_get_gfn(guest_l3e_t gl3e)
{ return _gfn(l3e_get_paddr(gl3e) >> PAGE_SHIFT); }
#if GUEST_PAGING_LEVELS >= 4
static inline gfn_t guest_l4e_get_gfn(guest_l4e_t gl4e)
{ return _gfn(l4e_get_paddr(gl4e) >> PAGE_SHIFT); }
#endif

static inline u32 guest_l1e_get_flags(guest_l1e_t gl1e)
{ return l1e_get_flags(gl1e); }
static inline u32 guest_l2e_get_flags(guest_l2e_t gl2e)
{ return l2e_get_flags(gl2e); }
static inline u32 guest_l3e_get_flags(guest_l3e_t gl3e)
{ return l3e_get_flags(gl3e); }
#if GUEST_PAGING_LEVELS >= 4
static inline u32 guest_l4e_get_flags(guest_l4e_t gl4e)
{ return l4e_get_flags(gl4e); }
#endif

static inline u32 guest_l1e_get_pkey(guest_l1e_t gl1e)
{ return l1e_get_pkey(gl1e); }
static inline u32 guest_l2e_get_pkey(guest_l2e_t gl2e)
{ return l2e_get_pkey(gl2e); }
static inline u32 guest_l3e_get_pkey(guest_l3e_t gl3e)
{ return l3e_get_pkey(gl3e); }

static inline guest_l1e_t guest_l1e_from_gfn(gfn_t gfn, u32 flags)
{ return l1e_from_pfn(gfn_x(gfn), flags); }
static inline guest_l2e_t guest_l2e_from_gfn(gfn_t gfn, u32 flags)
{ return l2e_from_pfn(gfn_x(gfn), flags); }
static inline guest_l3e_t guest_l3e_from_gfn(gfn_t gfn, u32 flags)
{ return l3e_from_pfn(gfn_x(gfn), flags); }
#if GUEST_PAGING_LEVELS >= 4
static inline guest_l4e_t guest_l4e_from_gfn(gfn_t gfn, u32 flags)
{ return l4e_from_pfn(gfn_x(gfn), flags); }
#endif

#define guest_l1_table_offset(a) l1_table_offset(a)
#define guest_l2_table_offset(a) l2_table_offset(a)
#define guest_l3_table_offset(a) l3_table_offset(a)
#define guest_l4_table_offset(a) l4_table_offset(a)

#endif /* GUEST_PAGING_LEVELS != 2 */

/* Mask of the GFNs covered by an L2 or L3 superpage */
#define GUEST_L2_GFN_MASK (GUEST_L1_PAGETABLE_ENTRIES - 1)
#define GUEST_L3_GFN_MASK \
    ((GUEST_L2_PAGETABLE_ENTRIES * GUEST_L1_PAGETABLE_ENTRIES) - 1)


/* Which pagetable features are supported on this vcpu? */

static inline int
guest_supports_superpages(struct vcpu *v)
{
    /* The _PAGE_PSE bit must be honoured in HVM guests, whenever
     * CR4.PSE is set or the guest is in PAE or long mode. 
     * It's also used in the dummy PT for vcpus with CR4.PG cleared. */
    return (is_pv_vcpu(v)
            ? opt_allow_superpage
            : (GUEST_PAGING_LEVELS != 2 
               || !hvm_paging_enabled(v)
               || (v->arch.hvm_vcpu.guest_cr[4] & X86_CR4_PSE)));
}

static inline int
guest_supports_1G_superpages(struct vcpu *v)
{
    return (GUEST_PAGING_LEVELS >= 4 && hvm_pse1gb_supported(v->domain));
}

static inline int
guest_supports_nx(struct vcpu *v)
{
    if ( GUEST_PAGING_LEVELS == 2 || !cpu_has_nx )
        return 0;
    if ( is_pv_vcpu(v) )
        return cpu_has_nx;
    return hvm_nx_enabled(v);
}


/*
 * Some bits are invalid in any pagetable entry.
 * Normal flags values get represented in 24-bit values (see
 * get_pte_flags() and put_pte_flags()), so set bit 24 in
 * addition to be able to flag out of range frame numbers.
 */
#if GUEST_PAGING_LEVELS == 3
#define _PAGE_INVALID_BITS \
    (_PAGE_INVALID_BIT | get_pte_flags(((1ull << 63) - 1) & ~(PAGE_SIZE - 1)))
#else /* 2-level and 4-level */
#define _PAGE_INVALID_BITS _PAGE_INVALID_BIT
#endif


/* Type used for recording a walk through guest pagetables.  It is
 * filled in by the pagetable walk function, and also used as a cache
 * for later walks.  When we encounter a superpage l2e, we fabricate an
 * l1e for propagation to the shadow (for splintering guest superpages
 * into many shadow l1 entries).  */
typedef struct guest_pagetable_walk walk_t;
struct guest_pagetable_walk
{
    unsigned long va;           /* Address we were looking for */
#if GUEST_PAGING_LEVELS >= 3
#if GUEST_PAGING_LEVELS >= 4
    guest_l4e_t l4e;            /* Guest's level 4 entry */
#endif
    guest_l3e_t l3e;            /* Guest's level 3 entry */
#endif
    guest_l2e_t l2e;            /* Guest's level 2 entry */
    guest_l1e_t l1e;            /* Guest's level 1 entry (or fabrication) */
#if GUEST_PAGING_LEVELS >= 4
    mfn_t l4mfn;                /* MFN that the level 4 entry was in */
    mfn_t l3mfn;                /* MFN that the level 3 entry was in */
#endif
    mfn_t l2mfn;                /* MFN that the level 2 entry was in */
    mfn_t l1mfn;                /* MFN that the level 1 entry was in */
};

/* Given a walk_t, translate the gw->va into the guest's notion of the
 * corresponding frame number. */
static inline gfn_t
guest_walk_to_gfn(walk_t *gw)
{
    if ( !(guest_l1e_get_flags(gw->l1e) & _PAGE_PRESENT) )
        return INVALID_GFN;
    return guest_l1e_get_gfn(gw->l1e);
}

/* Given a walk_t, translate the gw->va into the guest's notion of the
 * corresponding physical address. */
static inline paddr_t
guest_walk_to_gpa(walk_t *gw)
{
    if ( !(guest_l1e_get_flags(gw->l1e) & _PAGE_PRESENT) )
        return 0;
    return ((paddr_t)gfn_x(guest_l1e_get_gfn(gw->l1e)) << PAGE_SHIFT) +
           (gw->va & ~PAGE_MASK);
}

/* Given a walk_t from a successful walk, return the page-order of the 
 * page or superpage that the virtual address is in. */
static inline unsigned int 
guest_walk_to_page_order(walk_t *gw)
{
    /* This is only valid for successful walks - otherwise the 
     * PSE bits might be invalid. */
    ASSERT(guest_l1e_get_flags(gw->l1e) & _PAGE_PRESENT);
#if GUEST_PAGING_LEVELS >= 3
    if ( guest_l3e_get_flags(gw->l3e) & _PAGE_PSE )
        return GUEST_L3_PAGETABLE_SHIFT - PAGE_SHIFT;
#endif
    if ( guest_l2e_get_flags(gw->l2e) & _PAGE_PSE )
        return GUEST_L2_PAGETABLE_SHIFT - PAGE_SHIFT;
    return GUEST_L1_PAGETABLE_SHIFT - PAGE_SHIFT;
}


/* Walk the guest pagetables, after the manner of a hardware walker. 
 *
 * Inputs: a vcpu, a virtual address, a walk_t to fill, a 
 *         pointer to a pagefault code, the MFN of the guest's 
 *         top-level pagetable, and a mapping of the 
 *         guest's top-level pagetable.
 * 
 * We walk the vcpu's guest pagetables, filling the walk_t with what we
 * see and adding any Accessed and Dirty bits that are needed in the
 * guest entries.  Using the pagefault code, we check the permissions as
 * we go.  For the purposes of reading pagetables we treat all non-RAM
 * memory as contining zeroes.
 * 
 * Returns 0 for success, or the set of permission bits that we failed on 
 * if the walk did not complete. */

/* Macro-fu so you can call guest_walk_tables() and get the right one. */
#define GPT_RENAME2(_n, _l) _n ## _ ## _l ## _levels
#define GPT_RENAME(_n, _l) GPT_RENAME2(_n, _l)
#define guest_walk_tables GPT_RENAME(guest_walk_tables, GUEST_PAGING_LEVELS)

extern uint32_t 
guest_walk_tables(struct vcpu *v, struct p2m_domain *p2m, unsigned long va,
                  walk_t *gw, uint32_t pfec, mfn_t top_mfn, void *top_map);

/* Pretty-print the contents of a guest-walk */
static inline void print_gw(const walk_t *gw)
{
    gprintk(XENLOG_INFO, "GUEST WALK TO %p\n", _p(gw->va));
#if GUEST_PAGING_LEVELS >= 3 /* PAE or 64... */
#if GUEST_PAGING_LEVELS >= 4 /* 64-bit only... */
    gprintk(XENLOG_INFO, "   l4e=%" PRI_gpte " l4mfn=%" PRI_mfn "\n",
            gw->l4e.l4, mfn_x(gw->l4mfn));
    gprintk(XENLOG_INFO, "   l3e=%" PRI_gpte " l3mfn=%" PRI_mfn "\n",
            gw->l3e.l3, mfn_x(gw->l3mfn));
#else  /* PAE only... */
    gprintk(XENLOG_INFO, "   l3e=%" PRI_gpte "\n", gw->l3e.l3);
#endif /* PAE or 64... */
#endif /* All levels... */
    gprintk(XENLOG_INFO, "   l2e=%" PRI_gpte " l2mfn=%" PRI_mfn "\n",
            gw->l2e.l2, mfn_x(gw->l2mfn));
    gprintk(XENLOG_INFO, "   l1e=%" PRI_gpte " l1mfn=%" PRI_mfn "\n",
            gw->l1e.l1, mfn_x(gw->l1mfn));
}

#endif /* _XEN_ASM_GUEST_PT_H */
