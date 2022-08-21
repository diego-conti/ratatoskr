using namespace ratatoskr;

namespace Curvature {

	struct Parameters {
		unique_ptr<LieGroup> G;
		unique_ptr<PseudoRiemannianStructure> g;
		pair<int,int> signature;
		lst symbols;
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
		)(
			"generic-diagonal-metric", "generic diagonal metric", generic_diagonal_metric(&Parameters::g,&Parameters::G,&Parameters::symbols)
		)(
			"generic-metric", "generic metric all of whose nonzero entries are:", generic_metric_from_nonzero_entries (&Parameters::g,&Parameters::G,&Parameters::symbols)
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

}
