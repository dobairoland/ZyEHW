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

#define XML_ROOT ((const xmlChar *) "cgp")
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

static inline unsigned long xml_parse_decimal(const xmlChar *str)
{
        return strtoul((const char *) str, NULL, 10);
}

static inline unsigned long xml_parse_hexa(const xmlChar *str)
{
        return strtoul((const char *) str, NULL, 16);
}

static int check_cgp(const xmlNodePtr node)
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

static void xml_parse_lut_val(const xmlDocPtr doc, const xmlNodePtr pnode,
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

static void xml_parse_lut(const xmlDocPtr doc, const xmlNodePtr node,
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

static void xml_parse_pe(const xmlDocPtr doc, const xmlNodePtr pnode,
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

void xml_parse(const char *file, fitness_t *fit, cgp_t *cgp)
{
        xmlNodePtr node;
        xmlDocPtr doc = xmlParseFile(file);

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

        if (!check_cgp(node))
                return;

        cgp_initialize_pes(cgp);

        node = node->xmlChildrenNode;

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

        xmlFreeDoc(doc);

        /* cgp_print(cgp); */
}
