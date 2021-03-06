/*
Project Name : OpenMEEG

© INRIA and ENPC (contributors: Geoffray ADDE, Maureen CLERC, Alexandre
GRAMFORT, Renaud KERIVEN, Jan KYBIC, Perrine LANDREAU, Théodore PAPADOPOULO,
Emmanuel OLIVI
Maureen.Clerc.AT.sophia.inria.fr, keriven.AT.certis.enpc.fr,
kybic.AT.fel.cvut.cz, papadop.AT.sophia.inria.fr)

The OpenMEEG software is a C++ package for solving the forward/inverse
problems of electroencephalography and magnetoencephalography.

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's authors,  the holders of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#ifndef OPENMEEG_MESHDESCRIPTION_READER_SPECIALIZED_H
#define OPENMEEG_MESHDESCRIPTION_READER_SPECIALIZED_H

#include <vector>
#include <mesh3.h>
#include <MeshDescription/Reader.H>

namespace OpenMEEG {
    namespace MeshReader {

        struct VoidGeometry {
            typedef struct {} Type;
            static void Load(std::istream&) { }
        };

        struct MeshInterface {
            typedef Mesh Type;
            static void set_file_format(Type& interface,const std::string& name) { interface.get_file_format(name.c_str()); }
            static const char keyword[];
        };
        const char MeshInterface::keyword[] = "Mesh";

        struct Reader: public MeshDescription::Reader<MeshInterface,VoidGeometry> {

            typedef MeshDescription::Reader<MeshInterface,VoidGeometry> base;

        public:

            typedef MeshInterface::Type Mesh;
            typedef base::Domains       Domains;

            Reader(const char* geometry): base(geometry) { }

            std::vector<int> sortInterfaceIDAndDomains();
        };

        std::vector<int> Reader::sortInterfaceIDAndDomains() {
    #if 0
            for(int i=0;i<doms.size();i++){
                std::cout << "Domaine " << i << " : " << doms[i].name() << std::endl;
                for(int j=0;j<doms[i].size();j++){
                    std::cout << "\tInterfaceId : " << doms[i][j].interface() << std::endl;
                    std::cout << "\tInOut : " << doms[i][j].inout() << std::endl;
                }
            }
    #endif

            // Look for first internal surface :

            int firstSurfId = -1;
            for (Domains::iterator i=domains().begin();i!=domains().end();++i) {
                MeshDescription::Domain& domain = *i;
                if (domain.size()==1 && domain[0].inout()==MeshDescription::Inside) {
                    firstSurfId = domain[0].interface();
                    std::swap(domain,*domains().begin());
                    break;
                }
            }

            assert(firstSurfId>=0);

            std::vector<int> sortedListOfSurfId;

            // Sort surfaces from first internal to the most external :

            std::vector<bool> domainSeen(domains().size(),false);
            domainSeen[0] = true;
            std::vector<int> sortedDomainsId;
            sortedDomainsId.push_back(0);

            unsigned int currentExternalSurfaceId = firstSurfId;

            while (sortedDomainsId.size()!=domains().size())
                for (Domains::const_iterator i=domains().begin();i!=domains().end();++i) {
                    const unsigned ind = domains().index(*i);
                    if (!domainSeen[ind])
                        for (MeshDescription::Domain::const_iterator j=i->begin();j!=i->end();++j) {

                            // find the shared surface which is :
                            //   ** external for the last domain added
                            //   ** internal for the current domain
                            //   ** add the external surface to the list

                            if ((j->inout()==MeshDescription::Outside) && (j->interface()==currentExternalSurfaceId)) {
                                sortedListOfSurfId.push_back(currentExternalSurfaceId);
                                sortedDomainsId.push_back(ind);
                                domainSeen[ind] = true;
                                if (i->size()==2) {
                                    currentExternalSurfaceId = (++j!=i->end()) ? j->interface() : i->begin()->interface();
                                    break;
                                }
                            }
                        }
                }

            if (sortedListOfSurfId.size()!=interfaces().size()) {
                std::cout << "Current list : \t" ;
                for(size_t i=0;i<sortedListOfSurfId.size();i++)
                    std::cout << sortedListOfSurfId[i] << "\t" ;
                std::cerr << std::endl << "Cannot find " << interfaces().size();
                std::cerr << " nested interfaces with geometry file" << std::endl;
                exit(1);
            }

            // Reordering domains

            std::vector<MeshDescription::Domain> oldDomains = domains();
            for (Domains::iterator i=domains().begin();i!=domains().end();++i)
                *i = oldDomains[sortedDomainsId[domains().index(*i)]];

    #if 1
            std::cout << "Sorted List : \t" ;
            for(size_t i=0;i<sortedListOfSurfId.size();i++)
                std::cout << sortedListOfSurfId[i] << " " ;
            std::cout << std::endl;

            std::cout << "Sorted Domains : \t" ;
            for (Domains::const_iterator i=domains().begin();i!=domains().end();++i)
                std::cout << i->name() << "\t";
            std::cout << std::endl;
    #endif
            return sortedListOfSurfId;
        }
    }
}

#endif  //! OPENMEEG_MESHDESCRIPTION_READER_SPECIALIZED_H
