# Ratatoskr

`ratatoskr` is a header-only library built on top of [Wedge](https://github.com/diego-conti/wedge) to aid in writing command-line based programs to make computations on Lie algebras etc.

## Requirements

`ratatoskr` requires [cmake](https://cmake.org/) and [Wedge](https://github.com/diego-conti/wedge)

To install, run

	mkdir build
	cd build
	cmake ..
	cmake --build .
	cmake --install . --prefix=/where/you/want/it

You may need to set the WEDGE_PATH environment variable to point to your installation of Wedge, e.g.

	export WEDGE_PATH=/home/user/wedge
	
You can run some tests may be run by executing

	ctest --test-dir test

from the `build` directory.	

## Usage

A program built over `ratatoskr` consists of the following elements:

+ A struct, usually called `Parameters`, whose instances will be populated by reading the command-line parameters
+ A *parameter description*, obtained by invoking `make_parameter_description`, which instructs `ratatoskr` on how to populate the parameters object.
+ A function (or more generally, a callable object, e.g. a lambda) accepting an object of type `Parameters` and performing the actual computation.

#### Example

A program to compute the exterior derivative of a differential form on a Lie algebra can be written as follows

	struct Parameters {
		unique_ptr<LieGroup> G;
		ex form;
	};

	auto parameters_description=make_parameter_description<Parameters>
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"form","a differential form on the Lie algebra",differential_form(&Parameters::form,&Parameters::G)
	};
	auto program = make_program_description(
		"ExtDerivative", "Compute the exterior derivative of a form a on a Lie algebra",
		parameters_description, [] (Parameters& parameters) {
			cout<<latex<<parameters.G->d(parameters.form)<<endl;
		}
	);
		

### Parameter type

Types supported by Boost Program Options can be specified in the parameter description by `Parameters::&parameter`. This instructs `ratatoskr` to read them directly. These types include:

- numeric types such as `int`
- `bool` 
- `string`
- `vector`'s of the above. In the actual program invpcation, vectors should be passed as space-separated lists

Objects of some types introduced in Wedge can also appear in the parameter description. These types include:

- `AbstractLieGroup<false>`. This is the Wedge class for Lie groups not depending on parameters. Its description takes the form `lie_algebra(Parameters::&G)`, where `G` is a member of `Parameters` of type `unique_ptr<LieGroup>`, or possibly `unique_ptr<AbstractLieGroup<false>>`. For instance:

	struct Parameters {
		unique_ptr<LieGroup> G;
	};
	auto description_metric=make_parameter_description<Parameters>
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G)
	};
- `AbstractLieGroup<true>`. This is the Wedge class for Lie groups depending on parameters. Its description takes the form `lie_algebra(Parameters::&G, Parameters:&symbols)`, where `G` is a member of `Parameters` of type `unique_ptr<LieGroup>`, or possibly `unique_ptr<AbstractLieGroup<true>>`, and `symbols` is a member of `Parameters` of type `ratatoskr::GlobalSymbols`. For instance:

	struct Parameters {
		GlobalSymbols symbols;
		unique_ptr<LieGroup> G;
	};
	auto description_metric=make_parameter_description<Parameters>
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G, &Parameters::symbols)
	};

Notice that the parameter description does not contain any entry corresponding to `symbols`, which is implicitly initialiezed with a list of symbols that can be used in the definition of the Lie algebra.

- `PseudoRiemannianStructureByMatrix`. This is the Wedge class for pseudo-Riemannian metrics defined by a matrix. Rather than passing each individual entry of the matrix, `ratatoskr` allows the user to pass the metric by specifing the ♭ musical isomorphism, i.e. the comma-separated sequence *e*<sub>1</sub><sup>♭</sup>, ..., *e*<sub>m</sub><sup>♭</sup>. Each *e*<sub>i</sub><sup>♭</sup> is parsed as a differential form (see [differential form parsing](#differentialforms)) relative to the coframe *e<sup>1</sup>*, ..., *e<sup>n</sup>*.

- Differential forms. The C++ type for differential forms in Wedge is `ex` (the GiNaC type for arbitrary expressions). A differential form may be specified by `differential_form(&Parameters::G)`, where `G` is the parameter defining the Lie group in the `Parameters` struct.

	struct Parameters {
		unique_ptr<LieGroup> G;
		unique_ptr<PseudoRiemannianStructure> g;
	};
	auto description_metric=make_parameter_description<Parameters>
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"metric", "pseudo-riemannian metric on the Lie algebra", pseudo_riemannian_metric(&Parameters::g,&Parameters::G)
	);


Order is important in the parameter description. For instance, to define 

Order is irrelevant when invoking the program on the command line.