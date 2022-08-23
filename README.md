# Ratatoskr

`ratatoskr` is a header-only C++ library built on top of [Wedge](https://github.com/diego-conti/wedge) to aid in writing command-line based programs to make computations on Lie algebras relevant to differential geometry.

## Requirements and installation

`ratatoskr` requires [cmake](https://cmake.org/) and [Wedge](https://github.com/diego-conti/wedge)

To build, run

	mkdir build
	cd build
	cmake ..
	cmake --build .

This will build an executable called `ratatoskr` inside `build/ratatoskr`. The executable allows running a few simple programs, which are mainly included to illustrate how the library works, and will be used as examples in this documentation. 

You may need to set the WEDGE_PATH environment variable to point to your installation of Wedge, e.g.

	export WEDGE_PATH=/home/user/wedge
	
You can run some tests by executing

	ctest --test-dir test

from the `build` directory.	

You can install `ratatoskr` by running

	cmake --install . --prefix=/where/you/want/it

from the `build` directory.	 

## <a name="usage">Usage</a>

A program built over `ratatoskr` consists of the following elements:

+ A struct, usually called `Parameters`, whose instances will be populated by reading the command-line parameters
+ A *parameter description*, which instructs `ratatoskr` on how to populate the parameters object.
+ A function (or more generally, a callable object, e.g. a lambda)  which performs the actual computation and generates an output. The callable should accept an object of type `Parameters` which is populated based on the command line argument sand an object of type `ostream` which is used for output.

For instance, a program to compute the exterior derivative of a differential form on a Lie algebra can be written as follows

	#include "ratatoskr.h"
	using namespace ratatoskr;
	
	struct Parameters {
		unique_ptr<LieGroup> G;
		ex form;
	};
	auto parameters_description=make_parameter_description(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"form","a differential form on the Lie algebra",differential_form(&Parameters::form,&Parameters::G)
	};
	auto program = make_program_description(
		"ext-derivative", "Compute the exterior derivative of a form on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.G->d(parameters.form)<<endl;
		}
	);	

This program takes two command-line parameters, `--lie-algebra` and `--form` (see [differential form parsing](#differentialforms)), and prints out the exterior derivative of the form relative to the Lie algebra. 
	
More examples are available in the subdirectory `programs`. They can be tested by invoking the executable `ratatoskr`, e.g.:

	$ratatoskr/ratatoskr ext-derivative --lie-algebra 0,0,12 --form 3
	e1*e2

or, for latex output,

	$ratatoskr/ratatoskr ext-derivative --lie-algebra 0,0,12 --form 3 --latex	
	e^{12}


Notice that the option `--latex` is not indicated in the parameter description. It is an implicit option available for all programs generated with `make_program_description`, which has the effect of instructing the program to return latex output. 

Another implicit option is `--silent`, which discards all output. Future versions of `ratatoskr` may introduce more implicit options governing output.

### Program description

A C++ program may contain more than one 'program' in the sense explained in section [Usage](#usage). Each program is defined by an invocation of the form

	auto program=make_program_description(program_name, program_purpose, parameter_description, function);

where

- `program_name` is a string that does not contain any whitespace, giving the program name. It is used in error messages and by [`alternative_program_descriptions`](#alternativeprograms). 

- `program_purpose` is  a human-readable string illustrating what the program does, used in output and error messages.

- `parameter_description` is a variable obtained by invoking [`make_parameter_description`](#param_description), which instructs `ratatoskr` on how to populate the parameters from the command line.

- `function` is a callable object with signature.

`function(Parameters& parameters, ostream& output)`.

On execution, `ratatoskr` populates an object of type `Parameters` according to the contents of the command line and the parameter description, and executes the function, passing the parameters and a stream to be used for output.

Of course, the code above only defines a variable, and does not really run anything. A complete C++ program that actually runs the computation may be obtained by adding to the code the following `main` function:

	int main(int argc, char** argv) {
		program.run(argc,argv);
	}


### <a name="param_description">Parameter descriptions</a> 

A parameter description is created by invoking the template function `make_parameter_description` with a sequence of arguments of the form

	name, human-readable description, pointer-to-member|directive
	
where

- `name` is the command-line name of the parameter (not containing any whitespace). 
- `human-readable description` is a string that explains to the user what the parameter represents. 
-  a _pointer-to-member_ relative to the class `Parameters` can be specified, e.g. `Parameters::&parameter`, 
 instructs `ratatoskr` to read the parameter directly from the command line. This is only allowed for types supported by Boost Program Options, which include:
 	+ numeric types such as `int`, `float`, `double`
	+ `bool` 
	+ `string`
	+ `vector`'s of the above. In the actual program invocation, vectors should be passed as space-separated lists.

- a _directive_ is a function call that instructs `ratatoskr` on how to populate the parameter, based on the command line arguments; this method allows types that are not supported by Boost Program Options. For instance, a comma-separated pair of integers can be specified by the directive `comma_separated_pair(Parameters::&parameters)`; a GiNaC expression not including symbols can be described by the directive `expression(Parameters::&parameter)`. 

As an example to illustrate the two uses, consider the following program to convert between different units of measurements. The amount to convert is parsed as a floating point number, whereas the conversion rate is a GiNaC expression which is allowed to be a fraction.

	struct Parameters {
		float amount;
		ex conversion;
	};
	auto parameters_description=make_parameter_description(
		"amount","The amount to convert",&Parameters::amount,
		"conversion-ratio","The conversion ratio",expression(&Parameters::conversion)
	);
	auto program = make_program_description(
		"convert", "Convert to a different unit of measurement by applying a coefficient",
		parameters_description, [] (Parameters& parameters,ostream& os) {
			os<<parameters.conversion*parameters.amount<<endl;
		}
	);

	$ratatoskr/ratatoskr convert --amount 12.3 --conversion-ratio 6/5
	14.7600002288818359375


### Parameters depending on other parameters

For programs which require introducing new symbols, `ratatoskr` enables the creation of a new symbol with a name indicated in the command line. The parameter should be described by the directive `new_symbol(&Parameters::x)`, which has the effect of populating the member `x` of the struct `Parameters` with a new symbol. The class of the symbol can be optionally be specified as a template parameter, e.g. `new_symbol<realsymbol>(&Parameters::x)`. Notice that symbols should always be _stored_ as `ex` objects, no matter what type is used to define them.

For instance, the following program computes the derivative of a function of one variable.

	struct Parameters {
		ex function;
		ex variable;
	};
	auto parameters_description=make_parameter_description(
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

	auto parameters_description=make_parameter_description(
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
	auto parameters_description=make_parameter_description(
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
		auto parameters_description=make_parameter_description(
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
		auto description=make_parameter_description (
			"lie-algebra","Lie algebra with parameters",lie_algebra(&Parameters::G, &Parameters::symbols)
		};

	Alternatively:

		struct Parameters {
			ex symbols;
			unique_ptr<LieGroup> G;
		};
		auto description=make_parameter_description (
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

		auto parameters_description=make_parameter_description
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

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra with parameters",lie_algebra(&Parameters::G,&Parameters::symbols),
		"subalgebra","comma-separated list of generators of the subalgebra",lie_subalgebra(&Parameters::H,&Parameters::G,&Parameters::symbols)
	);

	auto program = make_program_description(
		"subalgebra-with-parameters", "Compute the structure constant of a Lie subalgebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.H->canonical_print(os)<<endl;
		}
	);
	
	Example usage:
	
		$ratatoskr/ratatoskr subalgebra-with-parameters --lie-algebra 0,0,[a]*12,13 --subalgebra [a]*1,4,2,3 --latex
		(0,a e^{14},0,a^2 e^{13})
	
## Metrics

There are two main classes used in Wedge to define a metric.

- `StandardPseudoRiemannianStructure`. This is the Wedge class for pseudo-Riemannian metrics defined by an orthonormal coframe. Since indefinite metrics are allowed, a signature needs to be specified. The directive to describe this parameter is

	`metric_by_on_coframe(&Parameters::metric, &Parameters::lie_algebra, &Parameters::signature).
	
	 The metric should be passed on the command line as a comma-separated list of one-forms. `signature` should be a pair of integers `(p,q)`. The first `p` entries in the coframe are assumed to be space-like, and the last `q` time-like.
		
- `PseudoRiemannianStructureByMatrix`. This is the Wedge class for pseudo-Riemannian metrics defined by a matrix. There are two ways to initialize an object of this type in `ratatoskr`. One applies to diagonal matrices, specified on the command line by the comma-separated list of the coefficients, and uses the directive
	
	diagonal_metric(&Parameters::g,&Parameters::G).	
	
For more general matrices, `ratatoskr` allows the user to pass the metric by specifing the ♭ musical isomorphism, i.e. the comma-separated sequence *e*<sub>1</sub><sup>♭</sup>, ..., *e*<sub>m</sub><sup>♭</sup>. Each *e*<sub>i</sub><sup>♭</sup> is parsed as a differential form (see [differential form parsing](#differentialforms)) relative to the coframe *e<sup>1</sup>*, ..., *e<sup>n</sup>*. This is obtained with the directive

	metric_by_flat(&Parameters::g,&Parameters::G)

### Alternative parameter descriptions

For some types of parameters, it makes sense to allow the user to choose between different ways of indicating the parameter on the command line. This can be achieved by using the `alternative` directive, which should be followed by a string summarizing the meaning of the parameters and a sequence of partial parameter descriptions, each enclosed in parentheses, i.e. 

	alternative(summary)(parameters1...)(parameters2...)
	
The alternative descriptions need not populate the same parameters, though they are required to refer to the same `Parameters` class.

Notice that this construct is only appropriate when the code that is being run by indicating each option is largely the same; otherwise, it  is advisable to use [`alternative_programs`](#alternativeprograms). 

For instance, we have seen that pseudo-Riemannian structures can be specified in three ways. These correspond to two different Wedge objects, which have a common ancestor `PseudoRiemannianStructure`; therefore, they can be viewed as alternative ways to construct a `unique_ptr<PseudoRiemannianStructure>` object. One can then define a program that allows the user to choose either way of specifying the metric, and then computes its curvature, as follows:

	struct Parameters {
		unique_ptr<LieGroup> G;
		unique_ptr<PseudoRiemannianStructure> g;
		pair<int,int> signature;
	};

	auto parameters_description=make_parameter_description (
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		alternative("define a pseudo-riemannian metric")(
			"signature", "signature (p,q)", comma_separated_pair(&Parameters::signature),
			"metric-by-on-coframe","metric defined by a comma-separated list of elements of an orthonormal coframe", metric_by_on_coframe(&Parameters::g,&Parameters::G,&Parameters::signature)
		)(
			"metric-by-flat", "metric defined by a comma-separated list of images of frame elements under flat isomorphism", metric_by_flat(&Parameters::g,&Parameters::G)
		)(
			"diagonal-metric", "metric identified by the comma-separated-list of diagonal elements of its matrix relative to the given frame", diagonal_metric(&Parameters::g,&Parameters::G)
		)
	);

	auto program = make_program_description(
		"curvature", "Compute the curvature of a pseudo-Riemannian metric on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.G->canonical_print(os)<<endl;
			PseudoLeviCivitaConnection omega(parameters.G.get(),*parameters.g);
			os<<"Curvature="<<omega.CurvatureForm()<<endl;
			os<<"Ricci tensor="<<omega.RicciAsMatrix()<<endl;
		}
	);
	
Example usage:

	$ratatoskr/ratatoskr curvature --lie-algebra 0,0,12,13  --signature=3,1 --metric-by-on-coframe "[1/sqrt(2)]*(1+3),2,4,[1/sqrt(2)]*(1-3)"
	(0,0,e1*e2,e1*e3)
	Curvature=[[0,-1/4*sqrt(2)*(e1*e4),-1/8*sqrt(2)*(e1*e4)-1/8*sqrt(2)*(e3*e4)-1/4*sqrt(2)*(e1*e2),3/4*(e1*e3)],[1/4*sqrt(2)*(e1*e4),0,0,1/4*sqrt(2)*(e1*e4)],[1/8*sqrt(2)*(e1*e4)+1/8*sqrt(2)*(e3*e4)+1/4*sqrt(2)*(e1*e2),0,0,-1/8*sqrt(2)*(e1*e4)+1/8*sqrt(2)*(e3*e4)+1/4*sqrt(2)*(e1*e2)],[3/4*(e1*e3),1/4*sqrt(2)*(e1*e4),-1/8*sqrt(2)*(e1*e4)+1/8*sqrt(2)*(e3*e4)+1/4*sqrt(2)*(e1*e2),0]]
	Ricci tensor=[[1/2,0,0,0],[0,0,0,0],[0,0,-1/2,0],[0,0,0,-1/2]]

	$ratatoskr/ratatoskr curvature --lie-algebra 0,0,12,13  --metric-by-flat 3,2,1,4	
	(0,0,e1*e2,e1*e3)
	Curvature=[[3/4*(e1*e3),0,0,-1/4*(e1*e4)],[1/2*(e1*e4),0,0,0],[0,-1/2*(e1*e4),-3/4*(e1*e3),-1/4*e3*e4-1/2*e1*e2],[1/4*e3*e4+1/2*e1*e2,0,1/4*(e1*e4),0]]
	Ricci tensor=[[0,0,1/2,0],[0,0,0,0],[1/2,0,0,0],[0,0,0,-1/2]]	

Notice that in both cases the metric is *e<sup>1</sup> ⊗ e<sup>3</sup>+e<sup>3</sup>⊗e<sup>1</sup>+e*<sup>2</sup>⊗e<sup>2</sup>+e*<sup>4</sup>⊗e<sup>4</sup>*, but the output differs because the class `PseudoLeviCivitaConnection` in Wedge uses the frame associated to the structure, which in the first case is the orthonormal frame.

### Creation of generic parameters

A common situation in computations is when a parameter is required to contain the generic element in some space, depending on generic symbols, e.g. the generic symmetrix matrix of order *n*. The following directives are available.

	generic_diagonal_metric(&Parameters::g,&Parameters::G,&Parameterd::metric_parameters)	
	
accepts the following arguments:
	
- `g` is a member of `Parameters` of type `unique_ptr<PseudoRiemannianStructure>` which will be populated with the metric.
- `G` is a member of `Parameters` of type `unique_ptr<LieGroup>` representing the Lie algebra.
- `metric_parameters` is a member of `Parameters`of type lst which will contain the symbols used to populate the metric.

This directive has the effect of populating `Parameters::g` with a generic diagonal metric. There are no command-line parameters associated to it.

	generic_metric_from_nonzero_entries	(&Parameters::g,&Parameters::G,&Parameterd::metric_parameters)	

accepts the following arguments:
	
- `g` is a member of `Parameters` of type `unique_ptr<PseudoRiemannianStructure>` which will be populated with the metric.
- `G` is a member of `Parameters` of type `unique_ptr<LieGroup>` representing the Lie algebra.
- `metric_parameters` is a member of `Parameters`of type lst which will contain the symbols used to populate the metric.

This directive has the effect of populating `Parameters::g` with the generic metric for which the only nonzero entries correspond to a fixed subset of the entries are zero. The pairs *row,column* corresponding to nonzero entries should be passed on the command line as a space separated list of comma-separated pairs, e.g.

	$ratatoskr/ratatoskr curvature --lie-algebra 0,0,12 --generic-metric 1,2 3,3
	(0,0,e1*e2)
	Curvature=[[3/4*a12^(-1)*(e1*e2)*a33,0,-1/4*a12^(-2)*(e1*e3)*a33^2],[0,-3/4*a12^(-1)*(e1*e2)*a33,-1/4*a12^(-2)*(e2*e3)*a33^2],[1/4*a12^(-1)*(e2*e3)*a33,1/4*a12^(-1)*(e1*e3)*a33,0]]
	Ricci tensor=[[0,1/2*a12^(-1)*a33,0],[1/2*a12^(-1)*a33,0,0],[0,0,-1/2*a12^(-2)*a33^2]]
	
	$ratatoskr/ratatoskr curvature --lie-algebra 0,0,12 --generic-diagonal-metric
	(0,0,e1*e2)
	Curvature=[[0,-3/4*g1^(-1)*(e1*e2)*g3,1/4*g1^(-1)*g2^(-1)*(e1*e3)*g3^2],[3/4*g2^(-1)*(e1*e2)*g3,0,1/4*g1^(-1)*g2^(-1)*(e2*e3)*g3^2],[-1/4*g2^(-1)*(e1*e3)*g3,-1/4*g1^(-1)*(e2*e3)*g3,0]]
	Ricci tensor=[[-1/2*g2^(-1)*g3,0,0],[0,-1/2*g1^(-1)*g3,0],[0,0,1/2*g1^(-1)*g2^(-1)*g3^2]]
	
Indices are one-based.

### <a name="alternativeprograms">Alternative programs and code reuse</a>

In situations where one has different programs which share some of the code or functionality, it may be practical to collect all code in a single directory, and treat the whole as an individual program (though of course code interdependence should be kept to a minimum, in order not to hamper code development and maintenance). This is the approach of the executable `ratatoskr/ratatoskr`, which performs some unrelated tasks, among which the user chooses by indicating the appropriate command-line parameter.

To achieve this, replace

	auto program = make_program_description("my program", ...);
	program.run(argc,argv);

with

	auto program1 = make_program_description("program1", ...);
	auto program2 = make_program_description("program2", ...);
		...

	auto programs = alternative_program_descriptions(program1, program2, ...);
	programs.run(argc,argv);
	
Running the executable without parameters has then the effect of printing a list of available programs. To run a specific program, pass the program name as the first command-line parameter (with no leading `--`), e.g.

	$./a.out program1 --options...

### Creating more directives

In order to create new directives, one can use the convenience function `generic_converter`. It is used as follows:

	generic_converter(&Parameters::p, converter, &Parameters::other_parameters...)
where
	
- `p` is the parameter that is being populated by this directory, call its type `T`
- `converter` is a callable object with signature

		T converter(const string& cmdline_parameter, other_parameters...)
	
which converts the string argument passed on the command line `cmdline_parameter` into an object of type `T`, optionally using other members of the `Parameters` object. 

- `other_parameters` is a sequence of zero or more pointers to members of `Parameters`. The corresponding elements of the `Parameters` object are passed as arguments to the `converter`.

It is also possible to create directives for parameters that are specified on the command line without an argument (like `--latex`). Such directives are created invoking

	generic_option(&Parameters::p, initializer, &Parameters::other_parameters...)

where	
- `p` is the parameter that is being populated by this directory, call its type `T`
- `initializer` is a callable object with signature

	T initializer(other_parameters...)
	
which initializes the parameter `Parameters::p`, optionally using other members of the `Parameters` object. 

- `other_parameters` is a sequence of zero or more pointers to members of `Parameters`. The corresponding elements of the `Parameters` object are passed as arguments to the `converter`.

### <a href="differentialforms">Parsing expressions and differential forms</a>

When parsing GiNaC expressions, `ratatoskr` uses the parsing functions [provided in GiNaC](https://www.ginac.de/tutorial/#Expression-input). When parsing differential forms, the parsing function `ParseDifferentialForm` of Wedge is used. Though this is a part of Wedge, it needs to be documented here to clarify how forms should be entered as command-line parameters inside programs employing  `ratatoskr`.

`ParseDifferentialforms` takes as arguments a coframe, a string to parse and optionally a list of GiNaC symbols. Is loosely based on the notation introduced in 

[S. Salamon: Complex structures on nilpotent Lie algebras. J. Pure Appl. Algebra 157 (2001), no. 2-3, 311--333.](#https://doi.org/10.1016/S0022-4049\(00\)00033-5)

If the coframe takes the form *e<sup>1</sup>,...., e<sup>n</sup>*:

- the digit *0* is intepreted as the zero differential form.
- a nonzero digit *i* is interpreted as the form *e<sup>i</sup>*. Hex digits are allowed, so this works up to dimension 15. 
- a sequence of digits *i<sub>1</sub> ... i<sub>n</sub>* is interpreted as the form *e<sup>i<sub>1</sub></sup> ∧ ... ∧ e<sup>i<sub>n</sub></sup>*
- an expression `coeff*form` is interpreted as a form multiplied by a coefficient. The coefficient can be one of the following
	+ an integer
	+ a fraction
	+ a GiNaC expression enclosed  in square brackets [...]. This expression may include symbols, square roots, etc.
	+ an expression in parentheses (...)
- `-form` is interpreted as `form` multiplied by -1.
- `form1+form2` is interpreted as the sum of the two forms
- `form1-form2` is interpreted as the difference of the two forms

For instance,

	-12+[a]*34+5*(6+7)+8(9+a)
	
determines the form *-e<sup>12</sup>+ a e<sup>34</sup>+5(e<sup>6</sup>+e<sup>7</sup>)+e<sup>8</sup>∧(e<sup>9</sup>+e<sup>10</sup>)*. Notice that for this to work, the symbol *a* (or a list containing it) must be passed as an argument when invoking `ParseDifferentialforms`.