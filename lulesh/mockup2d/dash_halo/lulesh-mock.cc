
#include <libdash.h>
#include <iostream>
#include <iomanip>
#include <vector>

#include "lulesh-mock.h"
#include "domain.h"

using std::cout;
using std::cerr;
using std::endl;

//
// The following four calls simulate all four communication scenarios
// found in LULESH:
//
// Call     #Fields   Node/Elem?   Direction  PlaneOnly?   Action
// ----------------------------------------------------------------
// NodalMass()   1        Node       Both       No            Add
// Force()       3        Node       Both       No            Add
// PosVel()      6        Node        One       No        Replace
// MonoQ()       3     Element       Both       Yes       Replace
//
// - #Fields is the number of scalar quantities (per node or element)
//      involved in the transfer. For example, PosVel transfers both
//      position (x, y, z) and velocity (xd, yd, zd);
//
// - Node/Elem denotes whether the call is concered with node-centered
//      or element centered fields.
//
// - Direction denotes whether communication occurs only in one
//      direction (propagation) or an actual exchange between
//      neighboring processes occurs.
//
// - PlaneOnly means whether communication only happens for planes in
//      3D (6 neighbors) or whether additionally edges and corners are
//      communicated as well (total of 26 neighbors). In the 2D
//      version of the mockup, PlaneOnly means communication only
//      happens on the edges.
//
// - Action means whether the communicated data from neighbors simply
//      replaces the locally stored data or if an addition of remote
//      values happens.
//

void Exchange_NodalMass(Domain& dom);
void Exchange_Force(Domain& dom);
void Exchange_PosVel(Domain& dom);
void Exchange_MonoQ(Domain& dom);


int main(int argc, char* argv[])
{
  dash::init(&argc, &argv);

  // 6x3 local elements and 2x4 process arrangement
  Domain dom({{6,3}}, {{2,4}});

  if( dom.numProcs()!=dom.px()*dom.py() ) {
    if( dom.myRank()==0 ) {
      std::cerr << "Err: this program must be run with " <<
	    dom.px()*dom.py() << " processes!" << std::endl;
    }
    dash::finalize();
    return 1;
  }

  dash::barrier();
  Exchange_NodalMass(dom);
  Exchange_Force(dom);
  //Exchange_PosVel(dom);
  //Exchange_MonoQ(dom);

  dash::finalize();
}


void Exchange_NodalMass(Domain& dom)
{
  dom.exchangeNodalMass();

  dash::barrier();
  dom.PrintNodalMass(1,1);
  dash::barrier();

}

void Exchange_Force(Domain& dom)
{
  dom.exchangeForce();

  dash::barrier();
  dom.PrintForce(1,1);
  dash::barrier();

}
/*
void Exchange_PosVel(Domain& dom)
{
  std::vector<Domain_member> fieldData;
  fieldData.push_back(&Domain::x);
  fieldData.push_back(&Domain::y);
  fieldData.push_back(&Domain::z);
  fieldData.push_back(&Domain::xd);
  fieldData.push_back(&Domain::yd);
  fieldData.push_back(&Domain::zd);

  dom.Exchange(dom.nNode(), fieldData,
	       Comm::Scope::All,
	       Comm::Direction::One,
	       Comm::Action::Replace);

  dash::barrier();
  dom.PrintPosVel(1,1);
  dash::barrier();

}

void Exchange_MonoQ(Domain& dom)
{
  std::vector<Domain_member> fieldData;
  fieldData.push_back(&Domain::delv_xi);
  fieldData.push_back(&Domain::delv_eta);
  fieldData.push_back(&Domain::delv_zeta);

  dom.Exchange(dom.nElem(), fieldData,
	       Comm::Scope::Plane,
	       Comm::Direction::Both,
	       Comm::Action::Replace);

  dash::barrier();
  dom.PrintMonoQ(1,1);
  dash::barrier();

}*/
