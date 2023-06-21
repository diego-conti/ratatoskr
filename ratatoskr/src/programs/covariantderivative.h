namespace CovariantDerivative {

	struct Parameters {
		unique_ptr<LieGroup> G;
		unique_ptr<PseudoRiemannianStructure> g;
		vector<int> timelike_indices;
		lst symbols;
		GlobalSymbols global_symbols;
		ex form;		
	};

	auto parameters_description=make_parameter_description (
		"lie-algebra","Lie algebra, possibly with parameters",lie_algebra(&Parameters::G,&Parameters::global_symbols),
		"form", "differential form", differential_form(&Parameters::form, &Parameters::G, &Parameters::global_symbols),
		alternative("define a pseudo-riemannian metric")(
			"metric-by-flat", "metric defined by a comma-separated list of images of frame elements under flat isomorphism", metric_by_flat(&Parameters::g,&Parameters::G)
		)(
			"diagonal-metric", "metric identified by the comma-separated-list of diagonal elements of its matrix relative to the given frame", diagonal_metric(&Parameters::g,&Parameters::G)
		)(
			"generic-diagonal-metric", "generic diagonal metric", generic_diagonal_metric(&Parameters::g,&Parameters::G,&Parameters::symbols)
		)(
			"generic-metric", "generic metric all of whose nonzero entries are:", generic_metric_from_nonzero_entries (&Parameters::g,&Parameters::G,&Parameters::symbols)
		)		
	);
	auto program = make_program_description(
		"covariant-derivative", "Compute covariant derivative of a differential form with respect to Levi-Civita on a pseudo-Riemannian metric on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.G->canonical_print(os)<<endl;
			PseudoLeviCivitaConnection omega(parameters.G.get(),*parameters.g);			
            for (auto X : parameters.G->e()) {                
                auto nabla_X_u=omega.Nabla<DifferentialForm>(X,parameters.form);
                os<<"\\nabla_{"<<X<<"}"<<parameters.form<<"="<<nabla_X_u<<endl;
            }
		}
	);

}
