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

Class
    Foam::basicSymmetryFvPatchField

Description
    A symmetry patch

SourceFiles
    basicSymmetryFvPatchField.C

\*---------------------------------------------------------------------------*/

#ifndef basicSymmetryFvPatchField_H
#define basicSymmetryFvPatchField_H

#include "transformFvPatchField.H"
#include "symmetryFvPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class basicSymmetryFvPatchField Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class basicSymmetryFvPatchField
:
    public transformFvPatchField<Type>
{

public:

    //- Runtime type information
    TypeName(symmetryFvPatch::typeName_());


    // Constructors

        //- Construct from patch and internal field
        basicSymmetryFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        basicSymmetryFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given basicSymmetryFvPatchField onto a new patch
        basicSymmetryFvPatchField
        (
            const basicSymmetryFvPatchField<Type>&,
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        basicSymmetryFvPatchField
        (
            const basicSymmetryFvPatchField<Type>&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchField<Type> > clone() const
        {
            return tmp<fvPatchField<Type> >
            (
                new basicSymmetryFvPatchField<Type>(*this)
            );
        }

        //- Construct as copy setting internal field reference
        basicSymmetryFvPatchField
        (
            const basicSymmetryFvPatchField<Type>&,
            const DimensionedField<Type, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchField<Type> > clone
        (
            const DimensionedField<Type, volMesh>& iF
        ) const
        {
            return tmp<fvPatchField<Type> >
            (
                new basicSymmetryFvPatchField<Type>(*this, iF)
            );
        }


    // Member functions

        // Evaluation functions

            //- Return gradient at boundary
            virtual tmp<Field<Type> > snGrad() const;

            //- Evaluate the patch field
            //  Default argument needed to allow call in constructors
            //  HJ, 30/Jun/2009
            virtual void evaluate
            (
                const Pstream::commsTypes commsType = Pstream::blocking
            );

            //- Return face-gradient transform diagonal
            virtual tmp<Field<Type> > snGradTransformDiag() const;
};


// * * * * * * * * * * * Template Specialisations  * * * * * * * * * * * * * //

template<>
tmp<scalarField > basicSymmetryFvPatchField<scalar>::snGrad() const;

template<>
void basicSymmetryFvPatchField<scalar>::evaluate
(
    const Pstream::commsTypes commsType
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "basicSymmetryFvPatchField.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
