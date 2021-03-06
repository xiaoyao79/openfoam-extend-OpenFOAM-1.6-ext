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

#include "DILUPreconditioner.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(DILUPreconditioner, 0);

    lduPreconditioner::
        addasymMatrixConstructorToTable<DILUPreconditioner>
        addDILUPreconditionerAsymMatrixConstructorToTable_;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::DILUPreconditioner::DILUPreconditioner
(
    const lduMatrix& matrix,
    const FieldField<Field, scalar>& coupleBouCoeffs,
    const FieldField<Field, scalar>& coupleIntCoeffs,
    const lduInterfaceFieldPtrsList& interfaces,
    const dictionary& 
)
:
    lduPreconditioner
    (
        matrix,
        coupleBouCoeffs,
        coupleIntCoeffs,
        interfaces
    ),
    rD_(matrix.diag())
{
    calcReciprocalD(rD_, matrix);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::DILUPreconditioner::calcReciprocalD
(
    scalarField& rD,
    const lduMatrix& matrix
)
{
    scalar* __restrict__ rDPtr = rD.begin();

    const label* const __restrict__ uPtr =
        matrix.lduAddr().upperAddr().begin();
    const label* const __restrict__ lPtr =
        matrix.lduAddr().lowerAddr().begin();

    const scalar* const __restrict__ upperPtr = matrix.upper().begin();
    const scalar* const __restrict__ lowerPtr = matrix.lower().begin();

    register label nFaces = matrix.upper().size();
    for (register label face=0; face<nFaces; face++)
    {
        rDPtr[uPtr[face]] -= upperPtr[face]*lowerPtr[face]/rDPtr[lPtr[face]];
    }


    // Calculate the reciprocal of the preconditioned diagonal
    register label nCells = rD.size();

    for (register label cell=0; cell<nCells; cell++)
    {
        rDPtr[cell] = 1.0/rDPtr[cell];
    }
}


void Foam::DILUPreconditioner::precondition
(
    scalarField& wA,
    const scalarField& rA,
    const direction
) const
{
    scalar* __restrict__ wAPtr = wA.begin();
    const scalar* __restrict__ rAPtr = rA.begin();
    const scalar* __restrict__ rDPtr = rD_.begin();

    const label* const __restrict__ uPtr =
        matrix_.lduAddr().upperAddr().begin();
    const label* const __restrict__ lPtr =
        matrix_.lduAddr().lowerAddr().begin();
    const label* const __restrict__ losortPtr =
        matrix_.lduAddr().losortAddr().begin();

    const scalar* const __restrict__ upperPtr = matrix_.upper().begin();
    const scalar* const __restrict__ lowerPtr = matrix_.lower().begin();

    register label nCells = wA.size();
    register label nFaces = matrix_.upper().size();
    register label nFacesM1 = nFaces - 1;

    for (register label cell=0; cell<nCells; cell++)
    {
        wAPtr[cell] = rDPtr[cell]*rAPtr[cell];
    }


    register label sface;

    for (register label face=0; face<nFaces; face++)
    {
        sface = losortPtr[face];
        wAPtr[uPtr[sface]] -=
            rDPtr[uPtr[sface]]*lowerPtr[sface]*wAPtr[lPtr[sface]];
    }

    for (register label face=nFacesM1; face>=0; face--)
    {
        wAPtr[lPtr[face]] -=
            rDPtr[lPtr[face]]*upperPtr[face]*wAPtr[uPtr[face]];
    }
}


void Foam::DILUPreconditioner::preconditionT
(
    scalarField& wT,
    const scalarField& rT,
    const direction
) const
{
    scalar* __restrict__ wTPtr = wT.begin();
    const scalar* __restrict__ rTPtr = rT.begin();
    const scalar* __restrict__ rDPtr = rD_.begin();

    const label* const __restrict__ uPtr =
        matrix_.lduAddr().upperAddr().begin();
    const label* const __restrict__ lPtr =
        matrix_.lduAddr().lowerAddr().begin();
    const label* const __restrict__ losortPtr =
        matrix_.lduAddr().losortAddr().begin();

    const scalar* const __restrict__ upperPtr = matrix_.upper().begin();
    const scalar* const __restrict__ lowerPtr = matrix_.lower().begin();

    register label nCells = wT.size();
    register label nFaces = matrix_.upper().size();
    register label nFacesM1 = nFaces - 1;

    for (register label cell=0; cell<nCells; cell++)
    {
        wTPtr[cell] = rDPtr[cell]*rTPtr[cell];
    }

    for (register label face=0; face<nFaces; face++)
    {
        wTPtr[uPtr[face]] -=
            rDPtr[uPtr[face]]*upperPtr[face]*wTPtr[lPtr[face]];
    }


    register label sface;

    for (register label face=nFacesM1; face>=0; face--)
    {
        sface = losortPtr[face];
        wTPtr[lPtr[sface]] -=
            rDPtr[lPtr[sface]]*lowerPtr[sface]*wTPtr[uPtr[sface]];
    }
}


// ************************************************************************* //
