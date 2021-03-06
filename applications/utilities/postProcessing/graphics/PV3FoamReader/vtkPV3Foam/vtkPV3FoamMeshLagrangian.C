/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Description

\*---------------------------------------------------------------------------*/

#include "vtkPV3Foam.H"

// Foam includes
#include "Cloud.H"
#include "fvMesh.H"
#include "IOobjectList.H"
#include "passiveParticle.H"
#include "vtkPV3FoamPoints.H"

// VTK includes
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

vtkPolyData* Foam::vtkPV3Foam::lagrangianVTKMesh
(
    const fvMesh& mesh,
    const word& cloudName
)
{
    vtkPolyData* vtkmesh = NULL;

    if (debug)
    {
        Info<< "<beg> Foam::vtkPV3Foam::lagrangianVTKMesh - timePath "
            << mesh.time().timePath()/cloud::prefix/cloudName << endl;
        printMemory();
    }


    // the region name is already in the mesh db
    IOobjectList sprayObjs
    (
        mesh,
        mesh.time().timeName(),
        cloud::prefix/cloudName
    );

    IOobject* positionsPtr = sprayObjs.lookup("positions");
    if (positionsPtr)
    {
        Cloud<passiveParticle> parcels(mesh, cloudName, false);

        if (debug)
        {
            Info<< "cloud with " << parcels.size() << " parcels" << endl;
        }

        vtkmesh = vtkPolyData::New();
        vtkPoints* vtkpoints = vtkPoints::New();
        vtkCellArray* vtkcells = vtkCellArray::New();

        vtkpoints->Allocate( parcels.size() );
        vtkcells->Allocate( parcels.size() );

        vtkIdType particleId = 0;
        forAllConstIter(Cloud<passiveParticle>, parcels, iter)
        {
            vtkPV3FoamInsertNextPoint(vtkpoints, iter().position());

            vtkcells->InsertNextCell(1, &particleId);
            particleId++;
        }

        vtkmesh->SetPoints(vtkpoints);
        vtkpoints->Delete();

        vtkmesh->SetVerts(vtkcells);
        vtkcells->Delete();
    }

    if (debug)
    {
        Info<< "<end> Foam::vtkPV3Foam::lagrangianVTKMesh" << endl;
        printMemory();
    }

    return vtkmesh;
}


// ************************************************************************* //
