/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
** ****************************************************************** */
//
// Description: This file contains the implementation of the
//              TclBasicBuilder_addFourNodeQuad() command.
//
// Written: fmk
// Created: 07/99
//
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <Domain.h>

#include <FourNodeQuad.h>
#include <FourNodeQuad3d.h>
#include <FourNodeQuadWithSensitivity.h>
#include <ConstantPressureVolumeQuad.h>
#include <EnhancedQuad.h>
#include <NineNodeMixedQuad.h>
#include <NineNodeQuad.h>
#include <EightNodeQuad.h>
#include <SixNodeTri.h>

class TclBasicBuilder;
#include <runtime/BasicModelBuilder.h>

int
TclBasicBuilder_addFourNodeQuad(ClientData clientData, Tcl_Interp *interp, int argc,
                                TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;


  if (builder->getNDM() != 2 || builder->getNDF() != 2) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with quad element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 8) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element FourNodeQuad eleTag? iNode? jNode? kNode? lNode? "
              "thk? type? matTag? <pressure? rho? b1? b2?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int FourNodeQuadId, iNode, jNode, kNode, lNode, matID;
  double thickness = 1.0;
  double p = 0.0;   // uniform normal traction (pressure)
  double rho = 0.0; // mass density
  double b1 = 0.0;
  double b2 = 0.0;

  if (Tcl_GetInt(interp, argv[argStart], &FourNodeQuadId) != TCL_OK) {
    opserr << "WARNING invalid FourNodeQuad eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[5 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
    return TCL_ERROR;
  }

  TCL_Char *type = argv[6 + argStart];

  if (Tcl_GetInt(interp, argv[7 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
    return TCL_ERROR;
  }

  if ((argc - argStart) > 11) {
    if (Tcl_GetDouble(interp, argv[8 + argStart], &p) != TCL_OK) {
      opserr << "WARNING invalid pressure\n";
      opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[9 + argStart], &rho) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[10 + argStart], &b1) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[11 + argStart], &b2) != TCL_OK) {
      opserr << "WARNING invalid b2\n";
      opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
      return TCL_ERROR;
    }
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == nullptr) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nFourNodeQuad element: " << FourNodeQuadId << endln;
    return TCL_ERROR;
  }

  // now create the FourNodeQuad and add it to the Domain
  FourNodeQuad *theFourNodeQuad =
      new FourNodeQuad(FourNodeQuadId, iNode, jNode, kNode, lNode, *theMaterial,
                       type, thickness, p, rho, b1, b2);

  if (builder->getDomain()->addElement(theFourNodeQuad) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
    delete theFourNodeQuad;
    return TCL_ERROR;
  }

  return TCL_OK;
}


int
TclBasicBuilder_addConstantPressureVolumeQuad(ClientData clientData,
                                              Tcl_Interp *interp, int argc,
                                              TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder->getNDM() != 2 || builder->getNDF() != 2) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with quad element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 7) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element ConstantPressureVolumeQuad eleTag? iNode? jNode? "
              "kNode? lNode? thk? matTag?\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int ConstantPressureVolumeQuadId, iNode, jNode, kNode, lNode, matID;
  double thickness = 1.0;

  if (Tcl_GetInt(interp, argv[argStart], &ConstantPressureVolumeQuadId) != TCL_OK) {
    opserr << "WARNING invalid ConstantPressureVolumeQuad eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "ConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "ConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "ConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "ConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[5 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "ConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[6 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "ConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    return TCL_ERROR;
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    return TCL_ERROR;
  }

  // now create the ConstantPressureVolumeQuad and add it to the Domain
  ConstantPressureVolumeQuad *theConstantPressureVolumeQuad =
      new ConstantPressureVolumeQuad(ConstantPressureVolumeQuadId, iNode, jNode,
                                     kNode, lNode, *theMaterial, thickness);
  if (theConstantPressureVolumeQuad == 0) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "ConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    return TCL_ERROR;
  }

  if (builder->getDomain()->addElement(theConstantPressureVolumeQuad) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "ConstantPressureVolumeQuad element: "
           << ConstantPressureVolumeQuadId << endln;
    delete theConstantPressureVolumeQuad;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}

int
TclBasicBuilder_addEnhancedQuad(ClientData clientData, Tcl_Interp *interp, int argc,
                                TCL_Char ** const argv)
{
  assert(clientData != nullptr);
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder->getNDM() != 2 || builder->getNDF() != 2) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with quad element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 8) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element EnhancedQuad eleTag? iNode? jNode? kNode? lNode? "
              "thk? type? matTag? \n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int EnhancedQuadId, iNode, jNode, kNode, lNode, matID;
  double thickness = 1.0;

  if (Tcl_GetInt(interp, argv[argStart], &EnhancedQuadId) != TCL_OK) {
    opserr << "WARNING invalid EnhancedQuad eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "EnhancedQuad element: " << EnhancedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "EnhancedQuad element: " << EnhancedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "EnhancedQuad element: " << EnhancedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "EnhancedQuad element: " << EnhancedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[5 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "EnhancedQuad element: " << EnhancedQuadId << endln;
    return TCL_ERROR;
  }

  TCL_Char *type = argv[6 + argStart];

  if (Tcl_GetInt(interp, argv[7 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "EnhancedQuad element: " << EnhancedQuadId << endln;
    return TCL_ERROR;
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nEnhancedQuad element: " << EnhancedQuadId << endln;
    return TCL_ERROR;
  }

  // now create the EnhancedQuad and add it to the Domain
  EnhancedQuad *theEnhancedQuad =
      new EnhancedQuad(EnhancedQuadId, iNode, jNode, kNode, lNode, *theMaterial,
                       type, thickness);

  if (builder->getDomain()->addElement(theEnhancedQuad) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "EnhancedQuad element: " << EnhancedQuadId << endln;
    delete theEnhancedQuad;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}

/*  *****************************************************************************

    N I N E   N O D E   M I X E D  Q U A D

    *****************************************************************************
 */

int
TclBasicBuilder_addNineNodeMixedQuad(ClientData clientData, Tcl_Interp *interp,
                                     int argc, TCL_Char ** const argv)
{
  // ensure the destructor has not been called -
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder->getNDM() != 2 || builder->getNDF() != 2) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with quad element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 11) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element NineNodeMixedQuad  eleTag?"
           << " iNode? jNode? kNode? lNode? mNode, nNode, pNode, qNode, "
              "centerNode "
           << " matTag?\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int NineNodeMixedQuadId, iNode, jNode, kNode, lNode;
  int mNode, nNode, pNode, qNode;
  int centerNode;
  int matID;

  if (Tcl_GetInt(interp, argv[argStart], &NineNodeMixedQuadId) != TCL_OK) {
    opserr << "WARNING invalid NineNodeMixedQuad eleTag" << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[5 + argStart], &mNode) != TCL_OK) {
    opserr << "WARNING invalid mNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[6 + argStart], &nNode) != TCL_OK) {
    opserr << "WARNING invalid nNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[7 + argStart], &pNode) != TCL_OK) {
    opserr << "WARNING invalid pNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[8 + argStart], &qNode) != TCL_OK) {
    opserr << "WARNING invalid qNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[9 + argStart], &centerNode) != TCL_OK) {
    opserr << "WARNING invalid centerNode\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[10 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nNineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  // now create the NineNodeMixedQuad and add it to the Domain
  NineNodeMixedQuad *theNineNodeMixed = new NineNodeMixedQuad(
      NineNodeMixedQuadId, iNode, jNode, kNode, lNode, mNode, nNode, pNode,
      qNode, centerNode, *theMaterial);

  if (theNineNodeMixed == 0) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    return TCL_ERROR;
  }

  if (builder->getDomain()->addElement(theNineNodeMixed) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "NineNodeMixedQuad element: " << NineNodeMixedQuadId << endln;
    delete theNineNodeMixed;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}

int
TclBasicBuilder_addFourNodeQuadWithSensitivity(ClientData clientData,
                                               Tcl_Interp *interp, int argc,
                                               TCL_Char ** const argv)
{
  // ensure the destructor has not been called -
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder == 0 || clientData == 0) {
    opserr << "WARNING builder has been destroyed\n";
    return TCL_ERROR;
  }

  if (builder->getNDM() != 2 || builder->getNDF() != 2) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with quad element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 8) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element FourNodeQuad eleTag? iNode? jNode? kNode? lNode? "
              "thk? type? matTag? <pressure? rho? b1? b2?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int FourNodeQuadId, iNode, jNode, kNode, lNode, matID;
  double thickness = 1.0;
  double p = 0.0; // uniform normal traction (pressure)
  double r = 0.0; // mass density
  double b1 = 0.0;
  double b2 = 0.0;

  if (Tcl_GetInt(interp, argv[argStart], &FourNodeQuadId) != TCL_OK) {
    opserr << "WARNING invalid FourNodeQuadWithSensitivity eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "FourNodeQuadWithSensitivity element: " << FourNodeQuadId
           << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "FourNodeQuadWithSensitivity element: " << FourNodeQuadId
           << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "FourNodeQuadWithSensitivity element: " << FourNodeQuadId
           << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "FourNodeQuadWithSensitivity element: " << FourNodeQuadId
           << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[5 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "FourNodeQuadWithSensitivity element: " << FourNodeQuadId
           << endln;
    return TCL_ERROR;
  }

  TCL_Char *type = argv[6 + argStart];

  if (Tcl_GetInt(interp, argv[7 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "FourNodeQuadWithSensitivity element: " << FourNodeQuadId
           << endln;
    return TCL_ERROR;
  }

  if ((argc - argStart) > 11) {
    if (Tcl_GetDouble(interp, argv[8 + argStart], &p) != TCL_OK) {
      opserr << "WARNING invalid pressure\n";
      opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[9 + argStart], &r) != TCL_OK) {
      opserr << "WARNING invalid rho\n";
      opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[10 + argStart], &b1) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[11 + argStart], &b2) != TCL_OK) {
      opserr << "WARNING invalid b2\n";
      opserr << "FourNodeQuad element: " << FourNodeQuadId << endln;
      return TCL_ERROR;
    }
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nFourNodeQuad element: " << FourNodeQuadId << endln;
    return TCL_ERROR;
  }

  // now create the FourNodeQuad and add it to the Domain
  FourNodeQuadWithSensitivity *theFourNodeQuadWithSensitivity =
      new FourNodeQuadWithSensitivity(FourNodeQuadId, iNode, jNode, kNode,
                                      lNode, *theMaterial, type, thickness, p,
                                      r, b1, b2);
  if (theFourNodeQuadWithSensitivity == 0) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "FourNodeQuadWithSensitivity element: " << FourNodeQuadId
           << endln;
    return TCL_ERROR;
  }

  if (builder->getDomain()->addElement(theFourNodeQuadWithSensitivity) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "FourNodeQuadWithSensitivity element: " << FourNodeQuadId
           << endln;
    delete theFourNodeQuadWithSensitivity;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}

// Regular nine node quad

int
TclBasicBuilder_addNineNodeQuad(ClientData clientData, Tcl_Interp *interp, int argc,
                                TCL_Char ** const argv)
{
  // TODO: assertions, clean up
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder == 0 || clientData == 0) {
    opserr << "WARNING builder has been destroyed\n";
    return TCL_ERROR;
  }

  if (builder->getNDM() != 2 || builder->getNDF() != 2) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with quad element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 13) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element NineNodeQuad eleTag? iNode? jNode? kNode? lNode? "
              "nNode? mNode? pNode? qNode? cNode? thk? type? matTag? "
              "<pressure? rho? b1? b2?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int NineNodeQuadId, iNode, jNode, kNode, lNode;
  int nNode, mNode, pNode, qNode, cNode, matID;
  double thickness = 1.0;
  double p = 0.0;   // uniform normal traction (pressure)
  double rho = 0.0; // mass density
  double b1 = 0.0;
  double b2 = 0.0;

  if (Tcl_GetInt(interp, argv[argStart], &NineNodeQuadId) != TCL_OK) {
    opserr << "WARNING invalid NineNodeQuad eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[5 + argStart], &nNode) != TCL_OK) {
    opserr << "WARNING invalid nNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[6 + argStart], &mNode) != TCL_OK) {
    opserr << "WARNING invalid mNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[7 + argStart], &pNode) != TCL_OK) {
    opserr << "WARNING invalid pNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[8 + argStart], &qNode) != TCL_OK) {
    opserr << "WARNING invalid qNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[9 + argStart], &cNode) != TCL_OK) {
    opserr << "WARNING invalid cNode\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[10 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  TCL_Char *type = argv[11 + argStart];

  if (Tcl_GetInt(interp, argv[12 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if ((argc - argStart) > 16) {
    if (Tcl_GetDouble(interp, argv[13 + argStart], &p) != TCL_OK) {
      opserr << "WARNING invalid pressure\n";
      opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[14 + argStart], &rho) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[15 + argStart], &b1) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[16 + argStart], &b2) != TCL_OK) {
      opserr << "WARNING invalid b2\n";
      opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
      return TCL_ERROR;
    }
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nNineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  // now create the NineNodeQuad and add it to the Domain
  NineNodeQuad *theNineNodeQuad = new NineNodeQuad(
      NineNodeQuadId, iNode, jNode, kNode, lNode, nNode, mNode, pNode, qNode,
      cNode, *theMaterial, type, thickness, p, rho, b1, b2);
  if (theNineNodeQuad == 0) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (builder->getDomain()->addElement(theNineNodeQuad) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "NineNodeQuad element: " << NineNodeQuadId << endln;
    delete theNineNodeQuad;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}

//
// Regular eight node quad
//
int
TclBasicBuilder_addEightNodeQuad(ClientData clientData, Tcl_Interp *interp,
                                 int argc, TCL_Char ** const argv)
{
  // ensure the destructor has not been called -
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder->getNDM() != 2 || builder->getNDF() != 2) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with quad element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 12) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element EightNodeQuad eleTag? iNode? jNode? kNode? lNode? "
              "nNode? mNode? pNode? qNode? thk? type? matTag? <pressure? rho? "
              "b1? b2?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int EightNodeQuadId, iNode, jNode, kNode, lNode;
  int nNode, mNode, pNode, qNode, matID;
  double thickness = 1.0;
  double p = 0.0;   // uniform normal traction (pressure)
  double rho = 0.0; // mass density
  double b1 = 0.0;
  double b2 = 0.0;

  if (Tcl_GetInt(interp, argv[argStart], &EightNodeQuadId) != TCL_OK) {
    opserr << "WARNING invalid EightNodeQuad eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[5 + argStart], &nNode) != TCL_OK) {
    opserr << "WARNING invalid nNode\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[6 + argStart], &mNode) != TCL_OK) {
    opserr << "WARNING invalid mNode\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[7 + argStart], &pNode) != TCL_OK) {
    opserr << "WARNING invalid pNode\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[8 + argStart], &qNode) != TCL_OK) {
    opserr << "WARNING invalid qNode\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[9 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  TCL_Char *type = argv[10 + argStart];

  if (Tcl_GetInt(interp, argv[11 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if ((argc - argStart) > 15) {
    if (Tcl_GetDouble(interp, argv[12 + argStart], &p) != TCL_OK) {
      opserr << "WARNING invalid pressure\n";
      opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[13 + argStart], &rho) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[14 + argStart], &b1) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[15 + argStart], &b2) != TCL_OK) {
      opserr << "WARNING invalid b2\n";
      opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
      return TCL_ERROR;
    }
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nEightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  // now create the EightNodeQuad and add it to the Domain
  EightNodeQuad *theEightNodeQuad = new EightNodeQuad(
      EightNodeQuadId, iNode, jNode, kNode, lNode, nNode, mNode, pNode, qNode,
      *theMaterial, type, thickness, p, rho, b1, b2);
  if (theEightNodeQuad == 0) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    return TCL_ERROR;
  }

  if (builder->getDomain()->addElement(theEightNodeQuad) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "EightNodeQuad element: " << EightNodeQuadId << endln;
    delete theEightNodeQuad;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}

// Six node tri

int
TclBasicBuilder_addSixNodeTri(ClientData clientData, Tcl_Interp *interp, int argc,
                              TCL_Char ** const argv)
{
  // ensure the destructor has not been called -
  BasicModelBuilder *builder = (BasicModelBuilder*)clientData;

  if (builder->getNDM() != 2 || builder->getNDF() != 2) {
    opserr << "WARNING -- model dimensions and/or nodal DOF not compatible "
              "with quad element\n";
    return TCL_ERROR;
  }

  // check the number of arguments is correct
  int argStart = 2;

  if ((argc - argStart) < 10) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: element SixNodeTri eleTag? iNode? jNode? kNode? lNode? "
              "nNode? mNode? pNode? qNode? thk? type? matTag? <pressure? rho? "
              "b1? b2?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int SixNodeTriId, iNode, jNode, kNode, lNode;
  int nNode, mNode, matID;
  double thickness = 1.0;
  double p = 0.0;   // uniform normal traction (pressure)
  double rho = 0.0; // mass density
  double b1 = 0.0;
  double b2 = 0.0;

  if (Tcl_GetInt(interp, argv[argStart], &SixNodeTriId) != TCL_OK) {
    opserr << "WARNING invalid SixNodeTri eleTag" << endln;
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[1 + argStart], &iNode) != TCL_OK) {
    opserr << "WARNING invalid iNode\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2 + argStart], &jNode) != TCL_OK) {
    opserr << "WARNING invalid jNode\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[3 + argStart], &kNode) != TCL_OK) {
    opserr << "WARNING invalid kNode\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[4 + argStart], &lNode) != TCL_OK) {
    opserr << "WARNING invalid lNode\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[5 + argStart], &nNode) != TCL_OK) {
    opserr << "WARNING invalid nNode\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[6 + argStart], &mNode) != TCL_OK) {
    opserr << "WARNING invalid mNode\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[7 + argStart], &thickness) != TCL_OK) {
    opserr << "WARNING invalid thickness\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  TCL_Char *type = argv[8 + argStart];

  if (Tcl_GetInt(interp, argv[9 + argStart], &matID) != TCL_OK) {
    opserr << "WARNING invalid matID\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  if ((argc - argStart) > 13) {
    if (Tcl_GetDouble(interp, argv[10 + argStart], &p) != TCL_OK) {
      opserr << "WARNING invalid pressure\n";
      opserr << "SixNodeTri element: " << SixNodeTriId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[11 + argStart], &rho) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "SixNodeTri element: " << SixNodeTriId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[12 + argStart], &b1) != TCL_OK) {
      opserr << "WARNING invalid b1\n";
      opserr << "SixNodeTri element: " << SixNodeTriId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[13 + argStart], &b2) != TCL_OK) {
      opserr << "WARNING invalid b2\n";
      opserr << "SixNodeTri element: " << SixNodeTriId << endln;
      return TCL_ERROR;
    }
  }

  NDMaterial *theMaterial = builder->getNDMaterial(matID);

  if (theMaterial == 0) {
    opserr << "WARNING material not found\n";
    opserr << "Material: " << matID;
    opserr << "\nSixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  // now create the SixNodeTri and add it to the Domain
  SixNodeTri *theSixNodeTri =
      new SixNodeTri(SixNodeTriId, iNode, jNode, kNode, lNode, nNode, mNode,
                     *theMaterial, type, thickness, p, rho, b1, b2);
  if (theSixNodeTri == nullptr) {
    opserr << "WARNING ran out of memory creating element\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    return TCL_ERROR;
  }

  if (builder->getDomain()->addElement(theSixNodeTri) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "SixNodeTri element: " << SixNodeTriId << endln;
    delete theSixNodeTri;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}

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
                                  int argc, TCL_Char ** const argv)
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
              "lNode? thk? matTag? bulk? rho? perm_x? perm_y? <b1? b2? "
              "pressure? dM? dK?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int FourNodeQuadUPId, iNode, jNode, kNode, lNode, matID;
  double thickness, bk, r, perm1, perm2;
  double p = 0.0; // uniform normal traction (pressure)
  double b1 = 0.0;
  double b2 = 0.0;

  // TCL_Char *type;
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

  if (builder->getDomain()->addElement(theFourNodeQuadUP) == false) {
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
                                      int argc, TCL_Char ** const argv)
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
        << "Want: element FourNodeQuadUP eleTag? Node1? ... Node9? thk? "
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

  if (builder->getDomain()->addElement(theNineFourNodeQuadUP) == false) {
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
                                      int argc, TCL_Char ** const argv)
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
              "thk? matTag? bulk? rho? perm_x? perm_y? <b1? b2? "
              "pressure? dM? dK?>\n";
    return TCL_ERROR;
  }

  // get the id and end nodes
  int BBarFourNodeQuadUPId, iNode, jNode, kNode, lNode, matID;
  double thickness, bk, r, perm1, perm2;
  double p = 0.0; // uniform normal traction (pressure)
  double b1 = 0.0;
  double b2 = 0.0;

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

  if (builder->getDomain()->addElement(theBBarFourNodeQuadUP) == false) {
    opserr << "WARNING could not add element to the domain\n";
    opserr << "BBarFourNodeQuadUP element: " << BBarFourNodeQuadUPId << endln;
    delete theBBarFourNodeQuadUP;
    return TCL_ERROR;
  }

  // if get here we have successfully created the element and added it to the
  // domain
  return TCL_OK;
}
