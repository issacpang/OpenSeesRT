//
// Description: This file contains the implementation of
// TclBasicBuilder_addFourNodeQuadUP() ,
// TclBasicBuilder_addNineFourNodeQuadUP() ,
// TclBasicBuilder_addBBarFourNodeQuadUP(),
//
// Zhaohui Yang and Jinchi Lu (September 2009)
//
#include <stdlib.h>
#include <string.h>
#include <Domain.h>

#include <FourNodeQuadUP.h>
#include <Nine_Four_Node_QuadUP.h>
#include <BBarFourNodeQuadUP.h>

class TclBasicBuilder;
#include <runtime/BasicModelBuilder.h>


/*  *****************************************************************************

    Q U A D  U_P

    *****************************************************************************
 */

int
TclBasicBuilder_addFourNodeQuadUP(ClientData clientData, Tcl_Interp *interp,
                                  int argc, TCL_Char ** const argv,
                                  Domain *theTclDomain,
                                  TclBasicBuilder *theTclBuilder)
{
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder->getNDM() != 2 || builder->getNDF() != 3) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with QuadUP element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 11) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element FourNodeQuadUP eleTag? iNode? jNode? kNode? "
              "lNode? thk? type? matTag? bulk? rho? perm_x? perm_y? <b1? b2? "
              "pressure? dM? dK?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int FourNodeQuadUPId, iNode, jNode, kNode, lNode, matID;
  double thickness, bk, r, perm1, perm2;
  double p = 0.0; // uniform normal traction (pressure)
  double b1 = 0.0;
  double b2 = 0.0;

  TCL_Char *type;
  if (Tcl_GetInt(interp, argv[argStart], &FourNodeQuadUPId) != TCL_OK) {
    opserr << "WARNING invalid FourNodeQuadUP eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[5 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[6 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[7 + argStart], &bk) != TCL_OK) {
    opserr << "WARNING invalid fluid bulk modulus\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[8 + argStart], &r) != TCL_OK) {
    opserr << "WARNING invalid fluid mass density\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[9 + argStart], &perm1) != TCL_OK) {
    opserr << "WARNING invalid lateral permeability\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[10 + argStart], &perm2) != TCL_OK) {
    opserr << "WARNING invalid vertical permeability\n";
    opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if ((argc - argStart) >= 12) {
    if (Tcl_GetDouble(interp, argv[11 + argStart], &b1) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }
  if ((argc - argStart) >= 13) {
    if (Tcl_GetDouble(interp, argv[12 + argStart], &b2) != TCL_OK) {
      opserr << "WARNING invalid b2\n";
      opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }
  if ((argc - argStart) >= 14) {
    if (Tcl_GetDouble(interp, argv[13 + argStart], &p) != TCL_OK) {
      opserr << "WARNING invalid pressure\n";
      opserr << "FourNodeQuadUP element: " << FourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nFourNodeQuadUP element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  // now create the FourNodeQuadUP and add it to the Domain
  FourNodeQuadUP *theFourNodeQuadUP = new FourNodeQuadUP(
      FourNodeQuadUPId, iNode, jNode, kNode, lNode, *theMaterial, "PlaneStrain",
      thickness, bk, r, perm1, perm2, b1, b2, p);
  if (theFourNodeQuadUP == 0) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (theTclDomain->addElement(theFourNodeQuadUP) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadUPId << endln;
    delete theFourNodeQuadUP;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}


/*  *****************************************************************************

    9-4-N O D E  Q U A D  U_P

    *****************************************************************************
 */

int
TclBasicBuilder_addNineFourNodeQuadUP(ClientData clientData, Tcl_Interp *interp,
                                      int argc, TCL_Char ** const argv,
                                      Domain *theTclDomain,
                                      TclBasicBuilder *theTclBuilder)
{
  // ensure the destructor has not been called -
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder == 0 || clientData == 0) {
    opserr << "WARNING builder has been destroyed\n";
    return TCL_ERROR;
  }

  if (builder->getNDM() != 2) {
    opserr << "WARNING -- model dimensions not compatible with 9-4-NodeQuadUP "
              "element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 16) {
    opserr << "WARNING insufficient arguments\n";
    opserr
        << "Want: element FourNodeQuadUP eleTag? Node1? ... Node9? thk? type? "
           "matTag? bulk? rho? perm_x? perm_y? <b1? b2? pressure? dM? dK?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int NineFourNodeQuadUPId, Node[9], matID;
  double thickness, bk, r, perm1, perm2;
  double b1 = 0.0;
  double b2 = 0.0;

  if (Tcl_GetInt(interp, argv[argStart], &NineFourNodeQuadUPId) != TCL_OK) {
    opserr << "WARNING invalid FourNodeQuadUP eleTag" << endln;
    return TCL_ERROR;
  }
  for (int i = 1; i <= 9; i++) {
    if (Tcl_GetInt(interp, argv[i + argStart], &Node[i - 1]) != TCL_OK) {
      opserr << "WARNING invalid Node\n";
      opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }

  if (Tcl_GetDouble(interp, argv[10 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[11 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[12 + argStart], &bk) != TCL_OK) {
    opserr << "WARNING invalid fluid bulk modulus\n";
    opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[13 + argStart], &r) != TCL_OK) {
    opserr << "WARNING invalid fluid mass density\n";
    opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[14 + argStart], &perm1) != TCL_OK) {
    opserr << "WARNING invalid lateral permeability\n";
    opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[15 + argStart], &perm2) != TCL_OK) {
    opserr << "WARNING invalid vertical permeability\n";
    opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if ((argc - argStart) >= 17) {
    if (Tcl_GetDouble(interp, argv[16 + argStart], &b1) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }
  if ((argc - argStart) >= 18) {
    if (Tcl_GetDouble(interp, argv[17 + argStart], &b2) != TCL_OK) {
      opserr << "WARNING invalid b2\n";
      opserr << "FourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nFourNodeQuadUP element: " << NineFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  // now create the FourNodeQuadUP and add it to the Domain
  NineFourNodeQuadUP *theNineFourNodeQuadUP = new NineFourNodeQuadUP(
      NineFourNodeQuadUPId, Node[0], Node[1], Node[2], Node[3], Node[4],
      Node[5], Node[6], Node[7], Node[8], *theMaterial, "PlaneStrain",
      thickness, bk, r, perm1, perm2, b1, b2);
  if (theNineFourNodeQuadUP == 0) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "FourNodeQuad element: " << NineFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (theTclDomain->addElement(theNineFourNodeQuadUP) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "FourNodeQuad element: " << NineFourNodeQuadUPId << endln;
    delete theNineFourNodeQuadUP;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}


/*  *****************************************************************************

    B B A R  Q U A D  U_P

    *****************************************************************************
 */

int
TclBasicBuilder_addBBarFourNodeQuadUP(ClientData clientData, Tcl_Interp *interp,
                                      int argc, TCL_Char ** const argv,
                                      Domain *theTclDomain,
                                      TclBasicBuilder *theTclBuilder)
{
  // ensure the destructor has not been called -
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder == 0 || clientData == 0) {
    opserr << "WARNING builder has been destroyed\n";
    return TCL_ERROR;
  }

  if (builder->getNDM() != 2 || builder->getNDF() != 3) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with QuadUP element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 11) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element bbarQuadUP eleTag? iNode? jNode? kNode? lNode? "
              "thk? type? matTag? bulk? rho? perm_x? perm_y? <b1? b2? "
              "pressure? dM? dK?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int BBarFourNodeQuadUPId, iNode, jNode, kNode, lNode, matID;
  double thickness, bk, r, perm1, perm2;
  double p = 0.0; // uniform normal traction (pressure)
  double b1 = 0.0;
  double b2 = 0.0;

  TCL_Char *type;
  if (Tcl_GetInt(interp, argv[argStart], &BBarFourNodeQuadUPId) != TCL_OK) {
    opserr << "WARNING invalid BBarFourNodeQuadUP eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[5 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[6 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[7 + argStart], &bk) != TCL_OK) {
    opserr << "WARNING invalid fluid bulk modulus\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[8 + argStart], &r) != TCL_OK) {
    opserr << "WARNING invalid fluid mass density\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[9 + argStart], &perm1) != TCL_OK) {
    opserr << "WARNING invalid lateral permeability\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[10 + argStart], &perm2) != TCL_OK) {
    opserr << "WARNING invalid vertical permeability\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if ((argc - argStart) >= 12) {
    if (Tcl_GetDouble(interp, argv[11 + argStart], &b1) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }
  if ((argc - argStart) >= 13) {
    if (Tcl_GetDouble(interp, argv[12 + argStart], &b2) != TCL_OK) {
      opserr << "WARNING invalid b2\n";
      opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }
  if ((argc - argStart) >= 14) {
    if (Tcl_GetDouble(interp, argv[13 + argStart], &p) != TCL_OK) {
      opserr << "WARNING invalid pressure\n";
      opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
      return TCL_ERROR;
    }
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == nullptr) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nBBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  // now create the BBarFourNodeQuadUP and add it to the Domain
  BBarFourNodeQuadUP *theBBarFourNodeQuadUP = new BBarFourNodeQuadUP(
      BBarFourNodeQuadUPId, iNode, jNode, kNode, lNode, *theMaterial,
      "PlaneStrain", thickness, bk, r, perm1, perm2, b1, b2, p);

  if (theBBarFourNodeQuadUP == nullptr) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    return TCL_ERROR;
  }

  if (theTclDomain->addElement(theBBarFourNodeQuadUP) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    delete theBBarFourNodeQuadUP;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}
