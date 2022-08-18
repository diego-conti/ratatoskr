# Ratatoskr

`ratatoskr` is a header-only library built on top of [Wedge](https://github.com/diego-conti/wedge) to aid in writing command-line based programs to make computations on Lie algebras etc.

## Requirements

`ratatoskr` requires [cmake](https://cmake.org/) and [Wedge](https://github.com/diego-conti/wedge)

To install, run

	mkdir build
	cd build
	cmake ..
	cmake --build .
	cmake --install . --prefix=/where/you/want/it (NOT IMPLEMENTED!!!!)

You may need to set the WEDGE_PATH environment variable to point to your installation of Wedge, e.g.

	export WEDGE_PATH=/home/user/wedge
	
You can run some tests may be run by executing

	ctest --test-dir test

from the `build` directory.	

## Usage

A program built over `ratatoskr` consists of the following elements:

+ A struct, usually called `Parameters`, whose instances will be populated by reading the command-line parameters
+ A *parameter description*, which instructs `ratatoskr` on how to populate the parameters object.
+ A function (or more generally, a callable object, e.g. a lambda)  which performs the actual computation and generates an output. The callable should accept an object of type `Parameters` which is populated based on the command line argument and an object of type `ostream` which is used for output.

For instance, a program to compute the exterior derivative of a differential form on a Lie algebra can be written as follows

	struct Parameters {
		unique_ptr<LieGroup> G;
		ex form;
	};
	auto parameters_description=make_parameter_description<Parameters>(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"form","a differential form on the Lie algebra",differential_form(&Parameters::form,&Parameters::G)
	};
	auto program = make_program_description(
		"ext-derivative", "Compute the exterior derivative of a form on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.G->d(parameters.form)<<endl;
		}
	);	

More examples are available in the subdirectory `programs`. They can be tested by invoking the executable `ratatoskr`, e.g.:

	$ratatoskr/ratatoskr ext-derivative --lie-algebra 0,0,12 --form 3
	e1*e2

or, for latex output,

	$ratatoskr/ratatoskr ext-derivative --lie-algebra 0,0,12 --form 3 --latex	
	e^{12}

Notice that the option `--latex` is not indicated in the parameter description. It is an implicit option available for all programs, which has the effect of instructing the program to return latex output. Future versions of `ratatoskr` may introduce more implicit options that govern output.

### Parameter descriptions

A parameter description is created by invoking the template function `make_parameter_description` with a sequence of arguments of the form

	name, human-readable description, pointer|directive

The name is the command-line name of the parameter. The human-readable description is a string that explains to the user what the parameter represents. 

The third argument instructs `ratatoskr` on how the parameter should be populated using the command line. 

- For types supported by Boost Program Options, a _pointer-to-member_ relative to the class `Parameters` can be specified, e.g. `Parameters::&parameter`. This instructs `ratatoskr` to read the parameter directly. Allowed types for this use include:

	+ numeric types such as `int`, `float`, `double`
	+ `bool` 
	+ `string`
	+ `vector`'s of the above. In the actual program invocation, vectors should be passed as space-separated lists.

- Types requiring an initialization or a conversions are specified by a _directive_. For instance, a GiNaC expression not including symbols can be described by the directive `expression(Parameters::&parameter)`. 

As an example to illustrate the two uses, consider the following program to convert between different units of measurements. The amount to convert is parsed as a floating point number, where as the conversion rate is a GiNaC expression which is allowed to be a fraction.

	struct Parameters {
		float amount;
		ex conversion;
	};
	auto parameters_description=make_parameter_description<Parameters>(
		"amount","The amount to convert",&Parameters::amount,
		"conversion-ratio","The conversion ratio",expression(&Parameters::conversion)
	);
	auto program = make_program_description(
		"convert", "Convert to a different unit of measurement by applying a coefficient",
		parameters_description, [] (Parameters& parameters,ostream& os) {
			os<<parameters.conversion*parameters.amount<<endl;
		}
	);
	
Example usage:

	$ratatoskr/ratatoskr convert --amount 12.3 --conversion-ratio 6/5
	14.7600002288818359375


### Parameters depending on other parameters

For programs which require introducing new symbols, `ratatoskr` enables the creation of a new symbol with a name indicated in the command line. The parameter should be described by the directive `new_symbol(&Parameters::x)`, which has the effect of populating the member `x` of the struct `Parameters` with a new symbol. The class of the symbol can be optionally be specified as a template parameter, e.g. `new_symbol<realsymbol>(&Parameters::x)`. Notice that symbols should always be _stored_ as `ex` objects, no matter what type is used to define them.

For instance, the following program computes the derivative of a function of one variable.

	struct Parameters {
		ex function;
		ex variable;
	};
	auto parameters_description=make_parameter_description<Parameters>(
		"variable","a variable",new_symbol(&Parameters::variable),
		"function","a function of one variable",expression(&Parameters::function,&Parameters::variable)
	);
	auto program = make_program_description(
		"derivative", "take the derivative of a function of one variable",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.function.diff(ex_to<symbol>(parameters.variable))<<endl;
		}
	);

Example usage:

	$ratatoskr/ratatoskr derivative --variable=x --function=x^2
	2*x

Notice that the two-parameter version of `expression` indicates that `function` _depends_ on the parameter `variable`. This means that the corresponding string will be converted to the associated symbol when parsing the command line argument. The general convention is that the first parameter in a directive indicates the parameter that is being described, and the remaining parameters refer to the other parameters it may depend on.

Order is important in the parameter description: any parameter should appear after every parameter it depends on, to ensure that initialization occurs in the proper order. However, order is irrelevant when invoking the program on the command line:

	$ratatoskr/ratatoskr derivative --function=x^2 --variable=x
	2 x

It is also possible to define more than one symbol with a single parameter, by using the directive `new_symbols` to describe a space-separated list of symbols, i.e.

	auto parameters_description=make_parameter_description<Parameters>(
		"variables","new variables",new_symbols(&Parameters::variables)
	);

In this example,  `Parameters::variables` should have type `lst` or `ex`.

## Parameters depending on global symbols

As an alternative to defining new symbols on the command line, one can opt to let `ratatoskr` define implicit, global symbols, and use those symbols consistently. This is done by adding an object of type `GlobalSymbols` to `Parameters`. Global symbols need not appear in the parameter description, since they are initialized implicitly. However, they may be appear in directives describing other parameters, which implies that the corresponding string will be associated with fixed symbols when parsing the corresponding command line arguments. Notice that if more  han one `GlobalSymbols` object is used, the symbols will only be instantiated once, and it makes no difference which `GlobalSymbols` symbol is used to access them.

For instance, the following program computes the partial derivative of a function of several variables. Notice that using global symbols allows the user to employ symbols which have not been specified on the command line.

	struct Parameters {
		GlobalSymbols symbols;
		ex function;
		ex variable;
	};
	auto parameters_description=make_parameter_description<Parameters>(
		"variable","a variable",expression(&Parameters::variable,&Parameters::symbols),
		"function","a function of one or more variables",expression(&Parameters::function,&Parameters::symbols)
	);
	auto program = make_program_description(
		"partial-derivative", "take a partial derivative of a function of more variables",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.function.diff(ex_to<symbol>(parameters.variable))<<endl;
		}
	);

Example usage:

	$ratatoskr/ratatoskr partial-derivative --variable=x --function=x^2*y
	2*x*y

Global symbols that are implicitly defined currently consists of single-letter symbols, either Roman or Greek.

### Lie algebras	

Lie algebras are represented in Wedge by subclasses of the abstract class `LieGroup`. Accordingly, they should be stored as pointers, i.e.

	struct Parameters {
		unique_ptr<LieGroup> G;
	};

If needed, `LieGroup` may be replaced by the actual subclass that is used to instantiate the object.

The different types of LieGroup in Wedge require different directives.

- `AbstractLieGroup<false>` is the Wedge class for Lie groups not depending on parameters. Use the directive `lie_algebra(Parameters::&G)`, for instance:

		struct Parameters {
			unique_ptr<AbstractLieGroup<false>> G;
			int p;
		};
		auto parameters_description=make_parameter_description<Parameters>(
			"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
			"p","a positive integer",&Parameters::p
		);
		auto program = make_program_description(
			"closed-forms", "Compute the space of closed p-forms",
			parameters_description, [] (Parameters& parameters, ostream& os) {
				os<<parameters.G->ClosedForms(parameters.p)<<endl;
			}
		);

	This defines a program to compute the closed forms on a Lie algebra without parameters. Example usage:
	
		$ratatoskr/ratatoskr closed-forms --lie-algebra 0,0,12 --p 2
		Subspace:{{
		e1*e2
		e1*e3
		e2*e3
		}}
		Complement:No elements
	
	To understand how the Lie algebra is encoded in the command line argument `--lie-algebra`, see [differential form parsing](#differentialforms).
	
- `AbstractLieGroup<true>` is the Wedge class for Lie groups depending on parameters. Use the directive `lie_algebra(Parameters::&G, Parameters:&symbols)`, where 
	+ `G` is a member of `Parameters` of type `unique_ptr<LieGroup>`, or possibly `unique_ptr<AbstractLieGroup<true>>`
	+ `symbols` is a member of `Parameters` of type `ex` or `GlobalSymbols`.
	
	For instance:

		struct Parameters {
			GlobalSymbols symbols;
			unique_ptr<LieGroup> G;
		};
		auto description=make_parameter_description<Parameters> (
			"lie-algebra","Lie algebra with parameters",lie_algebra(&Parameters::G, &Parameters::symbols)
		};

	Alternatively:

		struct Parameters {
			ex symbols;
			unique_ptr<LieGroup> G;
		};
		auto description=make_parameter_description<Parameters> (
			"symbols", "symbols to appear in Lie algebra description", new_symbols(&Parameters::symbols),
			"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G, &Parameters::symbols)
		};

- `AbstractLieSubgroup<false>`. This is the Wedge class for Lie subgroups not depending on parameters. Requires the directive
`lie_subalgebra(&Parameters::H, &Parameters::G)`, where

	- `H` is a member of `Parameters` of type `unique_ptr<LieGroup>` representing the subgroup
	- `G` is a member of `Parameters` of type `unique_ptr<LieGroup>` representing a group containing H

	The command-line parameter should be a comma-separated sequence of elements of the Lie algebra of `G`, representing the frame of the subalgebra. For instance, a program to write the structure constants of a subgroup can be written as follows:	
	
	struct Parameters {
		unique_ptr<LieGroupHasParameters<false>> G;
		unique_ptr<LieGroupHasParameters<false>> H;
	};

	auto parameters_description=make_parameter_description<Parameters>
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"subalgebra","comma-separated list of generators of the subalgebra",lie_subalgebra(&Parameters::H,&Parameters::G)
	);

	auto program = make_program_description(
		"subalgebra-without-parameters", "Compute the structure constant of a Lie subalgebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.H->canonical_print(os);
			os<<endl;
		}
	);
	
	Example usage:

		$ratatoskr/ratatoskr subalgebra-without-parameters --lie-algebra 0,0,12,13 --subalgebra 1,4,2,3 --latex
		(0,e^{14},0,e^{13})
	
- `AbstractLieSubgroup<true>`. This is the Wedge class for Lie subgroups depending on parameters. Its description takes the form 
`lie_subalgebra(&Parameters::H, &Parameters::G, &Parameters::symbols)`, where

	- `H` is a member of `Parameters` of type `unique_ptr<LieGroup>` representing the subgroup
	- `G` is a member of `Parameters` of type `unique_ptr<LieGroup>` representing a group containing H
	- `symbols` refers to either a `GlobalSymbols` or an `ex` of symbols generated by `new_symbols`

	A version of the `subalgebra` program that uses Lie group depending on parameters can be written as follows:
	
	struct Parameters {
		unique_ptr<LieGroupHasParameters<true>> G;
		unique_ptr<LieGroupHasParameters<true>> H;
		GlobalSymbols symbols;
	};

	auto parameters_description=make_parameter_description<Parameters>
	(
		"lie-algebra","Lie algebra with parameters",lie_algebra(&Parameters::G,&Parameters::symbols),
		"subalgebra","comma-separated list of generators of the subalgebra",lie_subalgebra(&Parameters::H,&Parameters::G,&Parameters::symbols)
	);

	auto program = make_program_description(
		"subalgebra-with-parameters", "Compute the structure constant of a Lie subalgebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.H->canonical_print(os);
			os<<endl;
		}
	);
	
	Example usage:
	
		$ratatoskr/ratatoskr subalgebra-with-parameters --lie-algebra 0,0,[a]*12,13 --subalgebra [a]*1,4,2,3 --latex
		(0,a e^{14},0,a^2 e^{13})
	
## Metrics

 `PseudoRiemannianStructureByMatrix`. This is the Wedge class for pseudo-Riemannian metrics defined by a matrix. Rather than passing each individual entry of the matrix, `ratatoskr` allows the user to pass the metric by specifing the ♭ musical isomorphism, i.e. the comma-separated sequence *e*<sub>1</sub><sup>♭</sup>, ..., *e*<sub>m</sub><sup>♭</sup>. Each *e*<sub>i</sub><sup>♭</sup> is parsed as a differential form (see [differential form parsing](#differentialforms)) relative to the coframe *e<sup>1</sup>*, ..., *e<sup>n</sup>*.

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


