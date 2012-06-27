/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2009 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#include "rptnetworkserializergui.h"

namespace voreen {

RptNetwork::RptNetwork()
  : reuseTCTargets(false), version(2), volumeSetContainer(0), serializeVolumeSetContainer(false)
{}

RptNetwork::~RptNetwork() {
    for (size_t i = 0; i < processorItems.size(); i++)
        delete processorItems[i];
}

RptNetwork* RptNetwork::load(const std::string& filename) {
    NetworkSerializer networkSerializer;
    ProcessorNetwork* network = networkSerializer.readNetworkFromFile(filename, false);
    RptNetwork* rptnet = RptNetworkSerializerGui::makeRptNetwork(network);
    delete network;
    return rptnet;
}

void RptNetwork::addProcessor(RptProcessorItem* processor) {
    VolumeSetSourceProcessor* vssp = dynamic_cast<VolumeSetSourceProcessor*>(processor->getProcessor());
    if (vssp)
        vssp->setVolumeSetContainer(volumeSetContainer);

    processorItems.push_back(processor);
}

bool RptNetwork::hasProcessor(RptProcessorItem* processor) {    
    for (size_t i = 0; i < processorItems.size(); i++)
        if (processorItems[i] == processor)
            return true;

    return false;
}

RptNetworkSerializerGui::RptNetworkSerializerGui() {}

int RptNetworkSerializerGui::readVersionFromFile(std::string filename) {
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
        throw SerializerException("Could not load network file!");
    int version = findVersion(&doc);
    return version;
}

int RptNetworkSerializerGui::findVersion(TiXmlNode* node) {
    if (node->Type() != TiXmlNode::ELEMENT) {
        TiXmlNode* pChild;
        for (pChild = node->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
            int result=findVersion(pChild);
            if (result!=0)
                return result;
        }
    } else {
        TiXmlElement* element=node->ToElement();
        if (element->FirstAttribute() ) {
            TiXmlAttribute* attribute=element->FirstAttribute();
            std::string s = attribute->Name();
            if (s == "Version" || s == "version")
                return attribute->IntValue();
        }
    }
    return 0;

}

void RptNetworkSerializerGui::serializeToXml(RptNetwork* rptnet, std::string filename) {
    ProcessorNetwork* net = makeProcessorNetwork(rptnet);
    NetworkSerializer networkserializer = NetworkSerializer();
    networkserializer.serializeToXml(net, filename);
}

ProcessorNetwork* RptNetworkSerializerGui::makeProcessorNetwork(RptNetwork* rptnet) {
    ProcessorNetwork* net = new ProcessorNetwork();
    net->reuseTCTargets = rptnet->reuseTCTargets;
    net->volumeSetContainer = rptnet->volumeSetContainer;
    net->serializeVolumeSetContainer = rptnet->serializeVolumeSetContainer;

    TiXmlElement* meta = new TiXmlElement("RptAggregationItems");

    // Get all the processors
    foreach (RptProcessorItem* item, rptnet->processorItems) {
        net->processors.push_back(item->saveMeta().getProcessor());
        // TODO Think about where to clean meta from old stuff
        if (net->processors.back()->hasInMeta("RptAggregationItem"))
            net->processors.back()->removeFromMeta("RptAggregationItem");
    }

    // Serialize aggregarions to metadata and get contained processors
    for (size_t i=0; i < rptnet->aggregationItems.size(); ++i) {
        // Serialize aggregation to metadata
        // TODO consider putting this code into the AggregationItem Class
        TiXmlElement* aggregationElement = new TiXmlElement("RptAggregationItem");
        aggregationElement->SetAttribute("id", i);
        aggregationElement->SetAttribute("name", rptnet->aggregationItems.at(i)->getName());
        aggregationElement->SetAttribute("x", static_cast<int>(rptnet->aggregationItems.at(i)->x()));
        aggregationElement->SetAttribute("y", static_cast<int>(rptnet->aggregationItems.at(i)->y()));
        meta->LinkEndChild(aggregationElement);
        // get processors in the aggregation
        foreach (RptProcessorItem* item, rptnet->aggregationItems.at(i)->getProcessorItems()) {
            net->processors.push_back(item->saveMeta().getProcessor());
            // add metadata for aggregations to processors
            TiXmlElement* aggregationElement = new TiXmlElement("RptAggregationItem");
            aggregationElement->SetAttribute("id", i);
            net->processors.back()->addToMeta(aggregationElement);
        }
    }

    // Get all the Property Sets
    for (size_t i=0; i< rptnet->propertySetItems.size(); i++)
        net->propertySets.push_back(rptnet->propertySetItems.at(i)->saveMeta().getPropertySet());

    // Add metadata
    net->addToMeta(meta);

    return net;
}

RptNetwork* RptNetworkSerializerGui::makeRptNetwork(ProcessorNetwork* net) {
    if (net->version < 2) throw AncientVersionException("This file version needs to be loaded with the old methods but they have been deleted!");
    RptNetwork* rptnet = new RptNetwork();
    rptnet->version = net->version;
    rptnet->reuseTCTargets = net->reuseTCTargets;
    rptnet->volumeSetContainer = net->volumeSetContainer;
    rptnet->serializeVolumeSetContainer = net->serializeVolumeSetContainer;

    //Prepare map that helps us with connections
    std::map<Processor*,RptProcessorItem*> processorMap;
    // Prepare Aggregations
    std::map< int, std::vector<RptProcessorItem*> > aggregationMap;
    // Iterate through Processors and create ProcessorItems
    for (size_t i = 0; i < net->processors.size(); ++i) {
        // create RptProcessorItems
        RptProcessorItem* processorItem = new RptProcessorItem(net->processors[i]);
        processorMap[net->processors[i]] = processorItem;
        processorItem->loadMeta();
        // if Item has aggregation metadata prepare to add it to aggregation
        if (net->processors[i]->hasInMeta("RptAggregationItem")) {
            TiXmlElement* aggregationElem = net->processors[i]->getFromMeta("RptAggregationItem");
            int id;
            aggregationElem->QueryIntAttribute("id", &id);
            aggregationMap[id].push_back(processorItem);
        }
        else
            rptnet->addProcessor(processorItem);
    }

    // Iterate through Processors again to create Connections between GuiItems
    for (size_t i = 0; i < net->processors.size(); ++i) {
        std::vector<Port*> outports = net->processors[i]->getOutports();
        std::vector<Port*> coprocessoroutports = net->processors.at(i)->getCoProcessorOutports();
        // append coprocessoroutports to outports because we can handle them identically FIXME is that really true?
        outports.insert(outports.end(), coprocessoroutports.begin(), coprocessoroutports.end());
        for (size_t j = 0; j < outports.size(); ++j) {
            // find all connections via this (out)port to (in)ports
            std::vector<Port*> connectedPorts = outports[j]->getConnected();
            for (size_t k = 0; k < connectedPorts.size(); ++k) {
                // get relevant data about connection
                Processor* processor = net->processors[i];
                Port* port = outports[j];
                std::string porttype = port->getType().getName();
                Port* connectedport = connectedPorts[k];
                Processor* connectedprocessor = connectedport->getProcessor();
                std::string connectedporttype = connectedport->getType().getName();
                // disconnect this connection
                // processor->disconnect(port, connectedport);
                // connect GuiItems (wich also reconnects the processors)
                processorMap[processor]->connectGuionly(
                        processorMap[processor]->getPortItem(porttype),
                        processorMap[connectedprocessor]->getPortItem(connectedporttype));
            }
        }
    }

    // create propertySetItems
    for (size_t i = 0; i < net->propertySets.size(); ++i) {
        RptPropertySetItem* propertySetItem = new RptPropertySetItem(net->propertySets[i], processorMap);
        propertySetItem->loadMeta();
        rptnet->propertySetItems.push_back(propertySetItem);
    }

    // Create AggregationItems
    TiXmlElement* rptAggregationItems = net->getFromMeta("RptAggregationItems");
    TiXmlElement* aggregationElem;
    for (aggregationElem = rptAggregationItems->FirstChildElement("RptAggregationItem");
    aggregationElem;
    aggregationElem = aggregationElem->NextSiblingElement("RptAggregationItem"))
    {
        int id;
        if (aggregationElem->QueryIntAttribute("id", &id) != TIXML_SUCCESS)
            throw XmlAttributeException("Required attribute 'id' missing on aggregation");
        float x,y;
        if (aggregationElem->QueryFloatAttribute("x",&x) != TIXML_SUCCESS ||
            aggregationElem->QueryFloatAttribute("y",&y) != TIXML_SUCCESS)
            throw XmlAttributeException("The Position of an AggregationItem remains unknown!");
        std::string name = aggregationElem->Attribute("name") ? aggregationElem->Attribute("name") : "Aggregation";
        if (aggregationMap[id].empty())
            throw XmlElementException("Something went wrong - we have an empty aggregation!");
        RptAggregationItem* aggregationItem = new RptAggregationItem(aggregationMap[id], name);
        aggregationItem->setPos(x,y);
        rptnet->aggregationItems.push_back(aggregationItem);
    }

    rptnet->errors = net->getErrors();
    return rptnet;
}

RptNetwork* RptNetworkSerializerGui::duplicateRptNetwork(const RptNetwork& rptnet) {
    RptNetwork rptnetcopy = rptnet;
    ProcessorNetwork* net = makeProcessorNetwork(&rptnetcopy); // copy here by assignment
    return makeRptNetwork(net);
}

} //namespace voreen