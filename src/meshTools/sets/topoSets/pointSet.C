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

\*---------------------------------------------------------------------------*/

#include "pointSet.H"
#include "mapPolyMesh.H"
#include "polyMesh.H"
#include "syncTools.H"

#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(pointSet, 0);

addToRunTimeSelectionTable(topoSet, pointSet, word);
addToRunTimeSelectionTable(topoSet, pointSet, size);
addToRunTimeSelectionTable(topoSet, pointSet, set);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

pointSet::pointSet(const IOobject& obj)
:
    topoSet(obj, typeName)
{}


pointSet::pointSet
(
    const polyMesh& mesh,
    const word& name,
    readOption r,
    writeOption w
)
:
    topoSet(mesh, typeName, name, r, w)
{
    check(mesh.nPoints());
}


pointSet::pointSet
(
    const polyMesh& mesh,
    const word& name,
    const label size,
    writeOption w
)
:
    topoSet(mesh, name, size, w)
{}


pointSet::pointSet
(
    const polyMesh& mesh,
    const word& name,
    const topoSet& set,
    writeOption w
)
:
    topoSet(mesh, name, set, w)
{}


pointSet::pointSet
(
    const polyMesh& mesh,
    const word& name,
    const labelHashSet& set,
    writeOption w
)
:
    topoSet(mesh, name, set, w)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

pointSet::~pointSet()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void pointSet::sync(const polyMesh& mesh)
{
    // Convert to boolList

    boolList contents(mesh.nPoints(), false);

    forAllConstIter(pointSet, *this, iter)
    {
        contents[iter.key()] = true;
    }
    syncTools::syncPointList
    (
        mesh,
        contents,
        orEqOp<bool>(),
        false,          // null value
        false           // no separation
    );

    // Convert back to labelHashSet

    labelHashSet newContents(size());

    forAll(contents, pointI)
    {
        if (contents[pointI])
        {
            newContents.insert(pointI);
        }
    }

    transfer(newContents);
}


label pointSet::maxSize(const polyMesh& mesh) const
{
    return mesh.nPoints();
}


void pointSet::updateMesh(const mapPolyMesh& morphMap)
{
    updateLabels(morphMap.reversePointMap());
}


void pointSet::writeDebug
(
    Ostream& os,
    const primitiveMesh& mesh,
    const label maxLen
) const
{
    topoSet::writeDebug(os, mesh.points(), maxLen);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
