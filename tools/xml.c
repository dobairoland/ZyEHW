/*
 * Copyright (C) 2014 Roland Dobai
 *
 * This file is part of ZyEHW.
 *
 * ZyEHW is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * ZyEHW is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with ZyEHW. If not, see <http://www.gnu.org/licenses/>.
 */

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "xml.h"

#if 0
#define XML_DEBUG
#endif

#define XML_ROOT ((const xmlChar *) "cgp_for_frames")
#define XML_CGP_ITEM ((const xmlChar *) "cgp")
#define XML_FRAME ((const xmlChar *) "frame")
#define XML_FITNESS ((const xmlChar *) "fitness")
#define XML_PE ((const xmlChar *) "pe")
#define XML_FILT_SW ((const xmlChar *) "filter_switch")
#define XML_OUT_SEL ((const xmlChar *) "out_select")
#define XML_A_MUX ((const xmlChar *) "a")
#define XML_B_MUX ((const xmlChar *) "b")
#define XML_LUT ((const xmlChar *) "l")
#define XML_LUT_ID ((const xmlChar *) "id")
#define XML_MSB ((const xmlChar *) "msb")
#define XML_LSB ((const xmlChar *) "lsb")
#define XML_COL ((const xmlChar *) "col")
#define XML_ROW ((const xmlChar *) "row")
#define XML_BIT ((const xmlChar *) "bit")

#define CGP_NO_FRAME    (-1)
#define CGP_LAST_FRAME  INT_MAX

static xmlDocPtr doc = NULL;
static xmlNodePtr cgp_nodes = NULL;
static cgp_t *cgp = NULL;
static int cgp_for_frame = CGP_NO_FRAME;
static int cgp_next_frame = CGP_NO_FRAME;

static inline unsigned long xml_parse_decimal(const xmlChar *str)
{
        return strtoul((const char *) str, NULL, 10);
}

static inline unsigned long xml_parse_hexa(const xmlChar *str)
{
        return strtoul((const char *) str, NULL, 16);
}

static int check_root_node(const xmlNodePtr node)
{
        xmlChar *attr = xmlGetProp(node, XML_COL);

        if (!attr) {
                fprintf(stderr, "COL cannot be found!\n");
                return 0;
        }

        if (xml_parse_decimal(attr) != CGP_COL) {
                fprintf(stderr, "XML: COLs don't match!\n");
                return 0;
        }

        xmlFree(attr);

        attr = xmlGetProp(node, XML_ROW);

        if (!attr) {
                fprintf(stderr, "ROW cannot be found!\n");
                return 0;
        }

        if (xml_parse_decimal(attr) != CGP_ROW) {
                fprintf(stderr, "XML: ROWs don't match!\n");
                return 0;
        }

        xmlFree(attr);

        attr = xmlGetProp(node, XML_BIT);

        if (!attr) {
                fprintf(stderr, "BIT cannot be found!\n");
                return 0;
        }

        if (xml_parse_decimal(attr) != CGP_BIT) {
                fprintf(stderr, "XML: BITs don't match!\n");
                return 0;
        }

        xmlFree(attr);

        return 1;
}

static unsigned long xml_parse_node_hexval(const xmlDocPtr doc,
                const xmlNodePtr node)
{
        unsigned long val = 0;
        xmlChar *str = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

        if (!str) {
                fprintf(stderr, "XML: Number not found in %s!\n", node->name);
                return val;
        }

        val = xml_parse_hexa(str);

        xmlFree(str);

        return val;
}

static inline fitness_t xml_parse_fitness(const xmlDocPtr doc,
                const xmlNodePtr node)
{
        return xml_parse_node_hexval(doc, node);
}

static int xmp_parse_pe_header(const xmlNodePtr node, int *col, int *row)
{
        xmlChar *attr = xmlGetProp(node, XML_COL);

        if (!attr) {
                fprintf(stderr, "PE COL cannot be found!\n");
                return 0;
        }

        *col = xml_parse_decimal(attr);

        if(*col >= CGP_COL || *col < 0) {
                fprintf(stderr, "XML: PE COL out of boudaries!\n");
                return 0;
        }

        xmlFree(attr);

        attr = xmlGetProp(node, XML_ROW);

        if (!attr) {
                fprintf(stderr, "PE ROW cannot be found!\n");
                return 0;
        }

        *row = xml_parse_decimal(attr);

        if (*row >= CGP_ROW || *row < 0) {
                fprintf(stderr, "XML: PE ROW out of boundaries!\n");
                return 0;
        }

        xmlFree(attr);

        return 1;
}

static inline void xml_parse_lut_val(const xmlDocPtr doc, const xmlNodePtr pnode,
                lut_t *msb, lut_t *lsb)
{
        xmlNodePtr node = pnode->xmlChildrenNode;

        if (!node) {
                fprintf(stderr, "LUT does not contain anything!\n");
                return;
        }

        for (; node != NULL; node = node->next) {
                if (!xmlStrcmp(node->name, XML_MSB))
                        *msb = xml_parse_node_hexval(doc, node);
                else if (!xmlStrcmp(node->name, XML_LSB))
                        *lsb = xml_parse_node_hexval(doc, node);
        }
}

static inline void xml_parse_lut(const xmlDocPtr doc, const xmlNodePtr node,
                cgp_pe_t *pe)
{
        int id;
        xmlChar *attr = xmlGetProp(node, XML_LUT_ID);

        if (!attr)
                fprintf(stderr, "LUT ID cannot be found!\n");

        id = xml_parse_decimal(attr);

        if(id >= CGP_BIT || id < 0)
                fprintf(stderr, "LUT ID out of boudaries!\n");

        xml_parse_lut_val(doc, node, &pe->msb[id], &pe->lsb[id]);

        xmlFree(attr);
}

static inline void xml_parse_pe(const xmlDocPtr doc, const xmlNodePtr pnode,
                cgp_t *cgp)
{
        cgp_pe_t *pe;
        int col, row;
        xmlNodePtr node = pnode->xmlChildrenNode;

        if (!node) {
                fprintf(stderr, "PE does not contain anything!\n");
                return;
        }

        if (!xmp_parse_pe_header(pnode, &col, &row))
                return;

        pe = &cgp->cgp_pe[col][row];

        for (; node != NULL; node = node->next) {
                if (!xmlStrcmp(node->name, XML_LUT))
                        xml_parse_lut(doc, node, pe);
                else if (!xmlStrcmp(node->name, XML_A_MUX))
                        pe->mux_a = xml_parse_node_hexval(doc, node);
                else if (!xmlStrcmp(node->name, XML_B_MUX))
                        pe->mux_b = xml_parse_node_hexval(doc, node);
        }
}

static inline void xml_parse_fil_sw(const xmlDocPtr doc, const xmlNodePtr node,
                cgp_t *cgp)
{
        cgp->filter_switch = xml_parse_node_hexval(doc, node);
}

static inline void xml_parse_out_sel(const xmlDocPtr doc, const xmlNodePtr node,
                cgp_t *cgp)
{
        cgp->out_select = xml_parse_node_hexval(doc, node);
}

static inline int xml_parse_frame(const xmlDocPtr doc, const xmlNodePtr node)
{
        return xml_parse_node_hexval(doc, node);
}

static inline void find_next_frame()
{
        cgp_next_frame = CGP_LAST_FRAME;

        for (; cgp_nodes != NULL; cgp_nodes = cgp_nodes->next) {
                if (!xmlStrcmp(cgp_nodes->name, XML_CGP_ITEM)) {
                        xmlNodePtr node = cgp_nodes->xmlChildrenNode;

                        for (; node != NULL; node = node->next) {
                                if (!xmlStrcmp(node->name, XML_FRAME)) {
                                        cgp_next_frame = xml_parse_frame(doc,
                                                        node);
#ifdef XML_DEBUG
                                        printf("Next frame will be 0x%X.\n",
                                                        cgp_next_frame);
#endif
                                        break;
                                }
                        }

                        if (cgp_next_frame <= cgp_for_frame) {
                                fprintf(stderr, "The frames are not in order."
                                                " The current frame is 0x%X "
                                                "and the next one is 0x%X!\n",
                                                cgp_for_frame,
                                                cgp_next_frame);
                        }
                        return;
                }
        }
}

int is_cgp_for_frame(int vframe)
{
        return vframe == cgp_for_frame;
}

const cgp_t *xml_parse(int vframe, fitness_t *fit)
{
        if (vframe >= cgp_next_frame) {
                xmlNodePtr node = cgp_nodes->xmlChildrenNode;

                cgp_initialize_pes(cgp);

                for (; node != NULL; node = node->next) {
                        if (!xmlStrcmp(node->name, XML_PE))
                                xml_parse_pe(doc, node, cgp);
                        else if (!xmlStrcmp(node->name, XML_FITNESS))
                                *fit = xml_parse_fitness(doc, node);
                        else if (!xmlStrcmp(node->name, XML_FILT_SW))
                                xml_parse_fil_sw(doc, node, cgp);
                        else if (!xmlStrcmp(node->name, XML_OUT_SEL))
                                xml_parse_out_sel(doc, node, cgp);
                }

#ifdef XML_DEBUG
                printf("CGP since frame 0x%X:\n", cgp_next_frame);
                cgp_print(cgp);
#endif
                cgp_for_frame = cgp_next_frame;
                cgp_nodes = cgp_nodes->next;
                find_next_frame();
        }

        return cgp;
}

void xml_start_parse(const char *file)
{
        xmlNodePtr node;

        doc = xmlParseFile(file);

        if (!doc) {
                fprintf(stderr, "XML parsing error!\n");
                return;
        }

        node = xmlDocGetRootElement(doc);

        if (!node) {
                fprintf(stderr, "Empty XML document!\n");
                return;
        }

        if (xmlStrcmp(node->name, XML_ROOT)) {
                fprintf(stderr, "XML root node incorrect!\n");
                xmlFreeDoc(doc);
                return;
        }

        if (!check_root_node(node))
                return;

        cgp_nodes = node->xmlChildrenNode;

        cgp = cgp_allocate();
        cgp_for_frame = CGP_NO_FRAME;

        find_next_frame();
}

void xml_end_parse()
{
        xmlFreeDoc(doc);
        cgp_deallocate(cgp);
}
