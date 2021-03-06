    Info<< "Reading mechanical properties\n" << endl;

    IOdictionary mechanicalProperties
    (
        IOobject
        (
            "mechanicalProperties",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    dimensionedScalar rho(mechanicalProperties.lookup("rho"));
    dimensionedScalar rhoE(mechanicalProperties.lookup("E"));
    dimensionedScalar nu(mechanicalProperties.lookup("nu"));

//    Info<< "Normalising E : E/rho\n" << endl;
//    dimensionedScalar E = rhoE/rho;
    dimensionedScalar E = rhoE;

    Info<< "Calculating Lame's coefficients\n" << endl;

    dimensionedScalar mu = E/(2.0*(1.0 + nu));
    dimensionedScalar lambda = nu*E/((1.0 + nu)*(1.0 - 2.0*nu));

    Switch planeStress(mechanicalProperties.lookup("planeStress"));

    if (planeStress)
    {
        Info<< "Plane Stress\n" << endl;

        //- change lambda for plane stress
        lambda = nu*E/((1.0 + nu)*(1.0 - nu));
    }
    else
    {
        Info<< "Plane Strain\n" << endl;
    }
    Info<< "mu = " << mu.value() << " Pa\n";
    Info<< "lambda = " << lambda.value() << " Pa\n";
