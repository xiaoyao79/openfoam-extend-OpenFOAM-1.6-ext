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

#include "lduMatrix.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineRunTimeSelectionTable(lduSmoother, symMatrix);
    defineRunTimeSelectionTable(lduSmoother, asymMatrix);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::word Foam::lduMatrix::smoother::getName
(
    const dictionary& dict
)
{
    word name;

    // handle primitive or dictionary entry
    const entry& e = dict.lookupEntry("smoother", false, false);
    if (e.isDict())
    {
        e.dict().lookup("smoother") >> name;
    }
    else
    {
        e.stream() >> name;
    }

    return name;
}


Foam::autoPtr<Foam::lduMatrix::smoother> Foam::lduMatrix::smoother::New
(
    const lduMatrix& matrix,
    const FieldField<Field, scalar>& coupleBouCoeffs,
    const FieldField<Field, scalar>& coupleIntCoeffs,
    const lduInterfaceFieldPtrsList& interfaces,
    const dictionary& dict
)
{
    word smootherName;

    // Handle primitive or dictionary entry
    const entry& e = dict.lookupEntry("smoother", false, false);
    if (e.isDict())
    {
        e.dict().lookup("smoother") >> smootherName;
    }
    else
    {
        e.stream() >> smootherName;
    }

    // Not (yet?) needed:
    // const dictionary& controls = e.isDict() ? e.dict() : dictionary::null;

    if (matrix.symmetric())
    {
        symMatrixConstructorTable::iterator constructorIter =
            symMatrixConstructorTablePtr_->find(smootherName);

        if (constructorIter == symMatrixConstructorTablePtr_->end())
        {
            FatalIOErrorIn
            (
                "lduMatrix::smoother::New\n"
                "(\n"
                "    const lduMatrix& matrix,\n"
                "    const FieldField<Field, scalar>& coupleBouCoeffs,\n"
                "    const FieldField<Field, scalar>& coupleIntCoeffs,\n"
                "    const lduInterfaceFieldPtrsList& interfaces,\n"
                "    const dictionary& dict\n"
                ")",
                dict
            )   << "Unknown symmetric matrix smoother " << smootherName
                << endl << endl
                << "Valid symmetric matrix smoothers are :" << endl
                << symMatrixConstructorTablePtr_->toc()
                << exit(FatalIOError);
        }

        return autoPtr<lduSmoother>
        (
            constructorIter()
            (
                matrix,
                coupleBouCoeffs,
                coupleIntCoeffs,
                interfaces
            )
        );
    }
    else if (matrix.asymmetric())
    {
        asymMatrixConstructorTable::iterator constructorIter =
            asymMatrixConstructorTablePtr_->find(smootherName);

        if (constructorIter == asymMatrixConstructorTablePtr_->end())
        {
            FatalIOErrorIn
            (
                "lduMatrix::smoother::New\n"
                "(\n"
                "    const lduMatrix& matrix,\n"
                "    const FieldField<Field, scalar>& coupleBouCoeffs,\n"
                "    const FieldField<Field, scalar>& coupleIntCoeffs,\n"
                "    const lduInterfaceFieldPtrsList& interfaces,\n"
                "    const dictionary& dict\n"
                ")",
                dict
            )   << "Unknown asymmetric matrix smoother " << smootherName
                << endl << endl
                << "Valid asymmetric matrix smoothers are :" << endl
                << asymMatrixConstructorTablePtr_->toc()
                << exit(FatalIOError);
        }

        return autoPtr<lduSmoother>
        (
            constructorIter()
            (
                matrix,
                coupleBouCoeffs,
                coupleIntCoeffs,
                interfaces
            )
        );
    }
    else
    {
        FatalIOErrorIn
        (
            "lduMatrix::smoother::New\n"
            "(\n"
            "    const lduMatrix& matrix,\n"
            "    const FieldField<Field, scalar>& coupleBouCoeffs,\n"
            "    const FieldField<Field, scalar>& coupleIntCoeffs,\n"
            "    const lduInterfaceFieldPtrsList& interfaces,\n"
            "    const dictionary& dict\n"
            ")",
            dict
        )   << "cannot solve incomplete matrix, "
               "no diagonal or off-diagonal coefficient"
            << exit(FatalIOError);

        return autoPtr<lduSmoother>(NULL);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::lduMatrix::smoother::smoother
(
    const lduMatrix& matrix,
    const FieldField<Field, scalar>& coupleBouCoeffs,
    const FieldField<Field, scalar>& coupleIntCoeffs,
    const lduInterfaceFieldPtrsList& interfaces
)
:
    matrix_(matrix),
    coupleBouCoeffs_(coupleBouCoeffs),
    coupleIntCoeffs_(coupleIntCoeffs),
    interfaces_(interfaces)
{}


// ************************************************************************* //
