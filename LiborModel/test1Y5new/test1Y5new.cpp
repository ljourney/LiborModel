/*
*
* The goal of this project is to calibrate blabla...
*
*
*
*
*
*/

#include "test1Y5new/test1Y5new.hpp"

void test1Y5new() {
	/*
	std::cout << "Testing calibration of a Libor forward model with 1Y5 settings..."
		<< std::endl;


	// basic settings
	SavedSettings backup;

	const Size size_ = 18;								// 24 trimesters
	const Real tolerance_ = 8e-3;						// tolerance

	boost::shared_ptr<IborIndex> libor = curveCreation();

	std::vector<swaptionData> swaptions					// swaption data
		= std::vector<swaptionData> {

			{ 50.200, Period(1, Months), Period(1, Years) },
			{ 54.475, Period(3, Months), Period(1, Years) },
			{ 63.350, Period(6, Months), Period(1, Years) },
			{ 68.650, Period(1, Years), Period(1, Years) },
			{ 49.850, Period(2, Years), Period(1, Years) },
			{ 38.500, Period(3, Years), Period(1, Years) },
			{ 31.900, Period(4, Years), Period(1, Years) },
			{ 28.500, Period(5, Years), Period(1, Years) },
			{ 60.950, Period(1, Months), Period(2, Years) },
			{ 55.700, Period(3, Months), Period(2, Years) },
			{ 58.100, Period(6, Months), Period(2, Years) },
			{ 56.550, Period(1, Years), Period(2, Years) },
			{ 42.600, Period(2, Years), Period(2, Years) },
			{ 34.300, Period(3, Years), Period(2, Years) },
			{ 29.400, Period(4, Years), Period(2, Years) },
			{ 51.050, Period(1, Months), Period(3, Years) },
			{ 48.300, Period(3, Months), Period(3, Years) },
			{ 48.900, Period(6, Months), Period(3, Years) },
			{ 47.000, Period(1, Years), Period(3, Years) },
			{ 37.000, Period(2, Years), Period(3, Years) },
			{ 31.275, Period(3, Years), Period(3, Years) },
			{ 40.800, Period(1, Months), Period(4, Years) },
			{ 40.225, Period(3, Months), Period(4, Years) },
			{ 40.200, Period(6, Months), Period(4, Years) },
			{ 39.300, Period(1, Years), Period(4, Years) },
			{ 32.800, Period(2, Years), Period(4, Years) },
			{ 36.025, Period(1, Months), Period(5, Years) },
			{ 35.600, Period(3, Months), Period(5, Years) },
			{ 35.425, Period(6, Months), Period(5, Years) },
			{ 34.250, Period(1, Years), Period(5, Years) },
			{ 30.800, Period(1, Months), Period(6, Years) },
			{ 30.700, Period(3, Months), Period(6, Years) },
			{ 31.200, Period(6, Months), Period(6, Years) },
			{ 30.750, Period(1, Years), Period(6, Years) }
	};

	Handle<YieldTermStructure> termStructure = libor->forwardingTermStructure();

	// set up the process
	boost::shared_ptr<LiborForwardModelProcess> process(
		new LiborForwardModelProcess(size_, libor));

	std::vector<Time> fixingT = process->fixingTimes();

	// correlation model
	boost::shared_ptr<PiecewiseConstantCorrelation> corrModel(
		new ExponentialForwardCorrelation(process->fixingTimes()));

	// Evolution Model
	EvolutionDescription ed(process->fixingTimes());

	// ctor
	//explicit EvolutionDescription(
	//	const std::vector<Time>& rateTimes,
	//	const std::vector<Time>& evolutionTimes = std::vector<Time>(),
	//	const std::vector<std::pair<Size, Size> >& relevanceRates =
	//	std::vector<range>());

	// sets the initial rates
	std::vector<Rate> rates, std::vector<Spread> displacements;

	for (std::vector<Time>::const_iterator It = process->fixingTimes().cbegin();
		It != process->fixingTimes().cend(); It++)
	{

		rates.push_back(termStructure->zeroRate(*It, Continuous).rate());
		displacements.push_back(.0001); // 1 bp
	}

	// creates the model
	//boost::shared_ptr<MarketModel> model(
	//	new AbcdVol(.5, .5, .5, .5, 
	//				process->fixingTimes(), // a checker 
	//				corrModel,
	//				ed, process->fixingTimes().size(), 
	//				rates));

	// abcdVol ctor
	//AbcdVol(
	//	Real a,
	//	Real b,
	//	Real c,
	//	Real d,
	//	const std::vector<Real>& ks,
	//	const boost::shared_ptr<PiecewiseConstantCorrelation>& corr,
	//	const EvolutionDescription& evolution,
	//	const Size numberOfFactors,
	//	const std::vector<Rate>& initialRates,
	//	const std::vector<Spread>& displacements);

	Size swapVolIndex = 0;
	DayCounter dayCounter = libor->forwardingTermStructure()->dayCounter();

	// set-up calibration helper
	std::vector<boost::shared_ptr<CalibrationHelper> > calibrationHelper;

	Size i;

	for (i = 0; i < swaptions.size(); i++) {

		Handle<Quote> swaptionVol(
			boost::shared_ptr<Quote>(
			new SimpleQuote(swaptions[i].volatility_ / 100)));

		boost::shared_ptr<CalibrationHelper> swaptionHelper(
			new SwaptionHelper(swaptions[i].lenght_,
			swaptions[i].maturity_, swaptionVol, libor,
			libor->tenor(), dayCounter,
			libor->dayCounter(),
			termStructure,
			CalibrationHelper::ImpliedVolError));

		swaptionHelper->setPricingEngine(
			boost::shared_ptr<PricingEngine>(
			new LfmSwaptionEngine(model, termStructure)));

		calibrationHelper.push_back(swaptionHelper);

	}

#ifdef _DEBUG

	LevenbergMarquardt om(1e-5, 1e-5, 1e-5);
	model->calibrate(calibrationHelper, om, EndCriteria(100, 20, 1e-5, 1e-5, 1e-6));

#else

	boost::shared_ptr<OptimizationMethod> om(
		new LevenbergMarquardt(1e-6, 1e-6, 1e-6));
	//boost::shared_ptr<OptimizationMethod> om(new SteepestDescent);
	model->calibrate(calibrationHelper, *om, EndCriteria(5000, 20, 1e-8, 1e-8, 1e-8));

#endif
	// measure the calibration error
	Real calculated = 0.0;
	for (i = 0; i<calibrationHelper.size(); ++i) {
		Real diff = calibrationHelper[i]->calibrationError();
		calculated += diff * diff;
	}

	// create diagnostic file
	{

		std::string fileStr("C:/Temp/liborModel_1Y5_");			// build file path
		fileStr.append(boost::posix_time::to_iso_string(
			boost::posix_time::second_clock::local_time()));
		fileStr.append(".csv");

		utilities::csvBuilder file(fileStr);				// csv builder

		Array times(size_, 0.0); Array rates(size_, 0.0);	// saves yield curve data

		for (int i = 0; i < size_; i++) {

			times[i] = fixingT[i];							// the fixing times from model
			rates[i] = libor->forwardingTermStructure()->zeroRate(
				times[i], Continuous);

		}

		file.add("times", 1, 1); file.add("rates", 1, 2);	// adds the yield curve data
		file.add(times, 2, 1); file.add(rates, 2, 2);

		file.add(std::string("calibration result:"), 1, 4);	// calibration result
		file.add(model->endCriteria(), 2, 4);

		file.add(std::string("calculated diff:"), 4, 4);	// calibration result
		file.add(std::sqrt(calculated), 5, 4);

		file.add("correlation matrix at time zero", 1, 6);	// correlation
		file.add(corrModel->correlation(0), 2, 6);
		
		

	}
	*/

}