
#include <cassert>
#include <cmath>
#include <memory>
#include <stdexcept>

#include <pacbio/consensus/ModelConfig.h>
#include <pacbio/consensus/Read.h>

#include "../ModelFactory.h"
#include "../Recursor.h"

namespace PacBio {
namespace Consensus {
namespace {

class SP1C1PwModel : public ModelConfig
{
    REGISTER_MODEL(SP1C1PwModel);

public:
    static std::set<std::string> Names() { return {"S/P1-C1", "S/P2-C2/prospective-compatible"}; }
    SP1C1PwModel(const SNR& snr);
    std::unique_ptr<AbstractRecursor> CreateRecursor(std::unique_ptr<AbstractTemplate>&& tpl,
                                                     const MappedRead& mr, double scoreDiff) const;
    std::vector<TemplatePosition> Populate(const std::string& tpl) const;
    double SubstitutionRate(uint8_t prev, uint8_t curr) const;

private:
    SNR snr_;
    double ctxTrans_[16][4];
};

REGISTER_MODEL_IMPL(SP1C1PwModel);

// TODO(lhepler) comments regarding the CRTP
class SP1C1PwRecursor : public Recursor<SP1C1PwRecursor>
{
public:
    SP1C1PwRecursor(std::unique_ptr<AbstractTemplate>&& tpl, const MappedRead& mr,
                    double scoreDiff);
    static inline std::vector<uint8_t> EncodeRead(const MappedRead& read);
    static inline double EmissionPr(MoveType move, uint8_t emission, uint8_t prev, uint8_t curr);
    virtual double UndoCounterWeights(size_t nEmissions) const;
};

double emissionPmf[3][16][12] = {
    {// matchPmf
        {    0.050180496,  0.000810831714,  0.000718815911,  2.41969643e-05,    0.0649534816,  0.000599364712,    0.0024105932,  2.79498377e-05,     0.872718151,    0.0038260229,   0.00356427001,   0.00014363133},
        {  0.00441418155,    0.0111340355,  0.000436100959,  0.000207054124,    0.0143053103,    0.0330608924,   0.00035144973,  0.000107614316,    0.0203264843,     0.914289415,  0.000516920114,  0.000820927935},
        { 0.000830704872,  4.01083601e-05,    0.0207832475,   0.00204041375,   0.00140918852,  1.59627818e-05,    0.0709485116,   0.00228655717,  0.000615064928,  0.000191122171,     0.890515556,    0.0102920349},
        { 0.000280390731,  0.000147856337,   0.00871749656,    0.0143671713,  0.000348528243,  3.09242691e-05,    0.0307957392,    0.0420922984,  0.000253817904,  0.000821599329,    0.0766740254,      0.82544661},
        {   0.0204497033,    0.0006522629,   0.00111751863,  6.88171386e-05,    0.0568321653,  0.000563436817,    0.0028379739,  2.22809772e-05,     0.910472162,   0.00273295039,   0.00398092872,  0.000246686918},
        {   0.0439247398,   0.00955997748,  0.000115883831,  0.000180023452,    0.0119201337,    0.0301506871,  0.000146989241,  0.000212785158,    0.0173905735,     0.886083207,  2.74319201e-05,  0.000259452999},
        { 0.000939303575,  5.20125284e-05,   0.00980339201,  0.000907334466,  0.000463702535,  1.79401825e-05,    0.0324806442,   0.00114187083,   0.00142047181,  0.000124132578,     0.944142586,   0.00848256955},
        { 0.000260915592,   0.00013834241,    0.0095663394,     0.016165433,  0.000260906504,   7.5920278e-05,    0.0322444797,    0.0433540894,  0.000653449354,  0.000422752406,    0.0760714376,     0.820755173},
        {  0.00956039032,  0.000314573628,  0.000370884177,  0.000117770504,     0.028518209,  0.000273935529,  0.000824887956,  3.66790527e-05,     0.953577007,   0.00398330966,   0.00222602028,  0.000169234775},
        {  0.00183055368,   0.00555526733,  7.32920566e-05,  0.000100825459,   0.00667577618,    0.0171484968,  0.000133252926,  0.000100846723,    0.0132914572,     0.954655196,  0.000133282582,  0.000280749525},
        {   0.0372218972,  3.13057624e-05,   0.00747417722,  0.000778927608,  0.000580843383,  9.37154184e-06,    0.0252638791,   0.00087834799,   0.00133003939,  0.000108090582,     0.919709258,    0.0065856977},
        { 7.34021416e-05,  2.11289101e-05,   0.00397571235,   0.00794336018,  0.000182826564,  4.28660053e-05,    0.0160848709,    0.0249312114,  0.000361781181,  0.000965491677,    0.0544096637,     0.890978452},
        {   0.0215150154,  0.000790146036,   0.00100949721,  0.000245739353,    0.0540046078,  0.000482950702,   0.00234925379,  3.35561393e-05,     0.911572324,    0.0044349381,   0.00338886125,  0.000137495886},
        {  0.00212710775,   0.00564615806,  0.000108665095,   8.3124319e-05,   0.00711710252,    0.0182314512,  0.000129199329,   0.00014006181,    0.0128209537,     0.953382861,  0.000111983946,    7.416168e-05},
        { 0.000714475349,  2.89513287e-05,   0.00993106299,  0.000963756992,  0.000625068525,   1.2084195e-05,     0.033994039,    0.0010809747,  0.000985106579,  0.000119201837,     0.944114814,   0.00740766497},
        {   0.0295954394,  0.000308518611,   0.00698927228,   0.00712426088,   0.00293094089,  0.000811966057,    0.0172195287,    0.0213957973,    0.0268789678,    0.0263604224,    0.0801385692,     0.780226789}},
    
    {// branchPmf
        {    0.367580288,   0.00016153169,   0.00016153169,   0.00016153169,     0.177171801,   0.00016153169,   0.00016153169,   0.00016153169,     0.452986467,   0.00016153169,   0.00016153169,   0.00016153169},
        { 0.000630798853,      0.12174078,  0.000630798853,  0.000630798853,  0.000630798853,     0.109459833,  0.000630798853,  0.000630798853,  0.000630798853,     0.759968203,  0.000630798853,  0.000630798853},
        {  0.00018211471,   0.00018211471,     0.436215655,   0.00018211471,   0.00018211471,   0.00018211471,     0.190416017,   0.00018211471,   0.00018211471,   0.00018211471,     0.370818723,   0.00018211471},
        {  0.00162252415,   0.00162252415,   0.00162252415,      0.30683009,   0.00162252415,   0.00162252415,   0.00162252415,     0.187965419,   0.00162252415,   0.00162252415,   0.00162252415,     0.482489153},
        {     0.22285451,  5.86536116e-05,  5.86536116e-05,  5.86536116e-05,     0.135841077,  5.86536116e-05,  5.86536116e-05,  5.86536116e-05,     0.640483263,  5.86536116e-05,  5.86536116e-05,  5.86536116e-05},
        { 0.000109110324,    0.0357894162,  0.000108808428,  0.000108808428,  0.000109086495,    0.0502780024,  0.000108808428,  0.000108808428,  0.000109274002,     0.912408218,  0.000108808428,  0.000108808428},
        {  0.00010725769,   0.00010725769,     0.344987237,   0.00010725769,   0.00010725769,   0.00010725769,     0.152744912,   0.00010725769,   0.00010725769,   0.00010725769,     0.500766243,   0.00010725769},
        {  0.00112670696,   0.00112670696,   0.00112670696,     0.211892718,   0.00112670696,   0.00112670696,   0.00112670696,     0.180261578,   0.00112670696,   0.00112670696,   0.00112670696,     0.592071807},
        {    0.181226885,    7.780819e-05,    7.780819e-05,    7.780819e-05,     0.102628527,    7.780819e-05,    7.780819e-05,    7.780819e-05,     0.715055273,    7.780819e-05,    7.780819e-05,    7.780819e-05},
        { 0.000232046724,    0.0678561269,  0.000232046724,  0.000232046724,  0.000232046724,    0.0826105386,  0.000232046724,  0.000232046724,  0.000232046724,      0.84628468,  0.000232046724,  0.000232046724},
        { 0.000152600774,  0.000152481727,      0.36057732,  0.000152481727,  0.000152671928,  0.000152481727,     0.161807588,  0.000152481727,  0.000161273012,  0.000152481727,     0.475471247,  0.000152481727},
        { 0.000452709553,  0.000452709553,  0.000452709553,     0.113042477,  0.000452709553,  0.000452709553,  0.000452709553,    0.0809680645,  0.000452709553,  0.000452709553,  0.000452709553,     0.799651525},
        {     0.16435014,  7.85392159e-05,  7.85392159e-05,  7.85392159e-05,     0.110607776,  7.85392159e-05,  7.85392159e-05,  7.85392159e-05,     0.723942535,  7.85392159e-05,  7.85392159e-05,  7.85392159e-05},
        { 0.000141875712,    0.0438774381,  0.000141875712,  0.000141875712,  0.000141875712,    0.0505647953,  0.000141875712,  0.000141875712,  0.000141875712,     0.903571507,  0.000141875712,  0.000141875712},
        { 8.49950399e-05,  8.49950399e-05,     0.255828845,  8.49950399e-05,  8.49950399e-05,  8.49950399e-05,     0.130922383,  8.49950399e-05,  8.49950399e-05,  8.49950399e-05,     0.612058841,  8.49950399e-05},
        { 7.34349915e-05,  5.19983486e-05,  5.19983486e-05,    0.0492213576,  5.20027988e-05,  5.19983486e-05,  5.19983486e-05,    0.0699666584,  5.36325917e-05,  5.19983486e-05,  5.19983486e-05,     0.880060932}},
    
    {// stickPmf
        { 0.000144273271,     0.031852115,     0.495294841,     0.038100898,  0.000144273271,     0.017212738,     0.122314015,    0.0167614466,  0.000144273271,    0.0753697914,     0.146214029,    0.0557259401},
        {    0.117962224,  6.59168851e-05,     0.282951648,    0.0192644475,     0.046904862,  6.59168851e-05,     0.116789946,    0.0191866887,     0.259151536,  6.59168851e-05,    0.0740004182,    0.0632608948},
        {      0.4239512,    0.0174098914,  0.000292385129,     0.157401908,    0.0812378375,   0.00996979079,  0.000292385129,    0.0478440513,     0.103765732,    0.0570054455,  0.000292385129,    0.0990750621},
        {    0.333696134,    0.0215995597,     0.257269536,  0.000137635044,    0.0854483376,    0.0098556168,     0.091392627,  0.000137635044,    0.0897605776,    0.0379132189,    0.0719633124,  0.000137635044},
        { 7.54578173e-05,    0.0166332203,     0.455649421,    0.0471517145,  7.54578173e-05,   0.00858907211,     0.128185013,   0.00844390932,  7.54578173e-05,     0.211699135,     0.088201127,    0.0348437253},
        {    0.112105594,  4.52902665e-05,     0.378558635,    0.0324997303,    0.0595842193,  4.26416883e-05,     0.195528879,    0.0131811878,    0.0831252123,  4.13209117e-05,    0.0934087306,    0.0316826901},
        {    0.340631586,     0.027496545,  0.000100648024,    0.0361256743,     0.114163109,    0.0172271722,  0.000100648024,    0.0151484488,     0.128820614,     0.256463889,  0.000100648024,    0.0631177767},
        {    0.249316656,     0.016756361,     0.179589221,  0.000102326946,    0.0850404612,    0.0131422017,    0.0646193548,  0.000102326946,    0.0825249453,     0.254973517,     0.053218666,  0.000102326946},
        {  0.00014006723,    0.0184767984,     0.541111101,    0.0356574565,   0.00014006723,   0.00737734189,      0.13451092,    0.0101765966,   0.00014006723,    0.0553982525,     0.124870347,    0.0713006475},
        {    0.089968542,  2.75232823e-05,     0.323766297,    0.0218632014,    0.0336158929,  2.75232823e-05,     0.215967877,    0.0116012507,    0.0329421856,  2.75232823e-05,     0.225971868,    0.0440826989},
        {    0.353010758,     0.028056778,    0.0141109415,    0.0541686994,     0.148416621,    0.0162924849,  0.000331214987,    0.0257340348,     0.132119269,    0.0676408297,  0.000260259152,     0.158619367},
        {    0.301161856,    0.0178563507,     0.274811467,  0.000154134305,    0.0974458666,   0.00739013642,     0.108102879,  0.000154134305,    0.0675909548,    0.0332511353,      0.09115628,  0.000154134305},
        { 0.000113490293,    0.0147487269,     0.305843505,      0.02221454,  0.000113490293,    0.0081304956,    0.0847889811,    0.0193666968,  0.000113490293,    0.0674361727,     0.184462101,     0.292100858},
        {    0.106375744,  5.18033726e-05,     0.247279422,    0.0217993491,    0.0454227851,  5.18033726e-05,    0.0926635587,    0.0197616719,    0.0520259221,  5.18033726e-05,     0.108375744,     0.305881377},
        {    0.281147231,    0.0141902129,  0.000105605946,    0.0328466806,    0.0817292326,   0.00930067567,  0.000105605946,    0.0288624596,    0.0768269567,    0.0393685594,  0.000105605946,     0.434883144},
        {    0.199657941,    0.0174846089,     0.257321115,  0.000317504884,    0.0573206577,   0.00455372773,     0.165110771,  9.56945913e-05,    0.0324444703,    0.0257359099,      0.23944167,   0.00010809618}}};

double transProbs[16][3][4] = {
    // Fit for context:  AA
    {
        { 4.64180060259476, -3.9536165441315, 0.629346482632806, -0.0333195939644658  },
        { 6.95397186355503, -4.81640615254612, 0.73957810252931, -0.0380629903397578  },
        { 2.0125609286762, -1.64230680477304, 0.188043347538493, -0.00800025350740117  }
    },
    // Fit for context:  AC
    {
        { -7.96643693768338, 2.45203940450617, -0.535064344077792, 0.0358830223158447  },
        { -8.7665502183788, 3.11090386888253, -0.502837045360252, 0.0268719645928884  },
        { 2.55241343397313, -2.61388872007515, 0.400198378643623, -0.0214289578490057  }
    },
    // Fit for context:  AG
    {
        { -8.95609979609381, 2.76776188942349, -0.448430889583276, 0.0238526474666025  },
        { 8.70083117131101, -6.14855240263022, 1.01305377841628, -0.0562353867193715  },
        { 3.85673355874382, -2.37323430285549, 0.253639486232327, -0.00792265363283865  }
    },
    // Fit for context:  AT
    {
        { 12.1647336238122, -7.9776855494273, 1.17170998397018, -0.057603362646467  },
        { 5.00290648719487, -4.16621645092003, 0.696297873836792, -0.0391611545344868  },
        { -4.01995319742413, 0.523828889049741, -0.103132759467356, 0.00520451145808958  }
    },
    // Fit for context:  CA
    {
        { -2.51102087348086, 0.204483340825312, -0.0649203448574841, 0.0049040915345933  },
        { 1.40048802441362, -2.24748845336743, 0.39759031823371, -0.0232098660636963  },
        { 6.25379124637344, -3.74840757828474, 0.503914277108986, -0.023498322774112  }
    },
    // Fit for context:  CC
    {
        { -8.45842695849197, 3.04649166320213, -0.537865339629986, 0.0303837875540666  },
        { -5.3848460306973, 1.68210750534009, -0.267218567372589, 0.0138107213099974  },
        { -6.92818052574026, 2.48746042364403, -0.454449677350371, 0.0258638165622547  }
    },
    // Fit for context:  CG
    {
        { -1.47733518453213, -0.60964075694068, 0.0706287595409263, -0.00243871775381204  },
        { -5.64639770150871, 1.63363359198718, -0.296877718476959, 0.0160551081644542  },
        { 1.32933319237265, -1.76650070845658, 0.229267885604329, -0.0108217271792876  }
    },
    // Fit for context:  CT
    {
        { -1.89298348233174, -1.05169004992836, 0.0785852195682609, 0.000648287464643558  },
        { 0.409528582048115, -1.53434275700689, 0.250882766974551, -0.0141427303266602  },
        { -0.814049625216239, -1.07671818027045, 0.171246935298451, -0.00990748002576864  }
    },
    // Fit for context:  GA
    {
        { 0.201195999563248, -1.46936914407633, 0.251926058146855, -0.0144336483641249  },
        { 1.15692566801376, -2.37338445578781, 0.422803587943207, -0.0249314131424101  },
        { -0.865159714636933, -0.530146528415917, -0.00126633169748894, 0.0023281565006292  }
    },
    // Fit for context:  GC
    {
        { -4.20369476733155, -0.403208361824981, 0.12053240324456, -0.0078924717222251  },
        { -7.07309386117761, 2.32339953661694, -0.344692279433874, 0.0170692678385147  },
        { -3.29462555451758, -0.0449183331796563, 0.0158592774136627, -0.00218563581174302  }
    },
    // Fit for context:  GG
    {
        { -6.618759868799, 1.77630213513116, -0.301917469878687, 0.0162987892702458  },
        { 6.46016885698086, -4.60291330456813, 0.71394376036697, -0.038926481203748  },
        { 4.1641434837661, -2.90948798940317, 0.413100412855066, -0.0198119106419827  }
    },
    // Fit for context:  GT
    {
        { 5.32979814815732, -4.36105624112514, 0.652808796272787, -0.0326266821355755  },
        { -1.7029081461207, -0.499477389809844, 0.0573393510996656, -0.00310784895624791  },
        { 7.11595788176173, -5.08972674986682, 0.801101205804288, -0.0414439829669328  }
    },
    // Fit for context:  TA
    {
        { -4.74271554330721, 1.28373596500549, -0.229225891578021, 0.0133403409911494  },
        { 2.22374687297304, -2.38792067161617, 0.373288541637948, -0.0191516834711714  },
        { 3.66108514125149, -2.59128954758056, 0.337444749378416, -0.0160370039528165  }
    },
    // Fit for context:  TC
    {
        { 2.04612192357648, -2.52151599689975, 0.379682953937167, -0.0179838619408694  },
        { -1.31673402810369, -0.562278453209614, 0.107691140486616, -0.00666343387997336  },
        { -4.94003805413098, 1.01280215159965, -0.19017869162516, 0.0108150675409149  }
    },
    // Fit for context:  TG
    {
        { 4.79424434531782, -4.02399375007546, 0.682218296797831, -0.0376455420429643  },
        { 5.75033593156235, -4.10145870919526, 0.636217309073862, -0.033566881903926  },
        { -0.267152607541904, -0.732160305934965, 0.029848264522247, 0.0010401530454877  }
    },
    // Fit for context:  TT
    { 
        { -5.68426500741552, 1.35382620376114, -0.177669648153777, 0.0069662442934688  },
        { -1.02037267928727, -0.844797584296236, 0.130687255561928, -0.00722481027088629  },
        { -0.376565549274208, -1.21268955791849, 0.178640505929895, -0.00928721253355987  } 
    }

};

SP1C1PwModel::SP1C1PwModel(const SNR& snr) : snr_(snr)
{
    const double snr1 = snr_.A, snr2 = snr1 * snr1, snr3 = snr2 * snr1;
    for (int ctx = 0; ctx < 16; ++ctx) {
        double sum = 1.0;
        ctxTrans_[ctx][0] = 1.0;
        for (size_t j = 0; j < 3; ++j) {
            double xb = transProbs[ctx][j][0] + snr1 * transProbs[ctx][j][1] +
                        snr2 * transProbs[ctx][j][2] + snr3 * transProbs[ctx][j][3];
            xb = std::exp(xb);
            ctxTrans_[ctx][j + 1] = xb;
            sum += xb;
        }
        for (size_t j = 0; j < 4; ++j)
            ctxTrans_[ctx][j] /= sum;
    }
}

std::unique_ptr<AbstractRecursor> SP1C1PwModel::CreateRecursor(
    std::unique_ptr<AbstractTemplate>&& tpl, const MappedRead& mr, double scoreDiff) const
{
    return std::unique_ptr<AbstractRecursor>(
        new SP1C1PwRecursor(std::forward<std::unique_ptr<AbstractTemplate>>(tpl), mr, scoreDiff));
}

std::vector<TemplatePosition> SP1C1PwModel::Populate(const std::string& tpl) const
{
    std::vector<TemplatePosition> result;

    if (tpl.empty()) return result;

    result.reserve(tpl.size());

    // Calculate probabilities in all 16 Contexts
    uint8_t prev = detail::TranslationTable[static_cast<uint8_t>(tpl[0])];
    if (prev > 3) throw std::invalid_argument("invalid character in template!");

    for (size_t i = 1; i < tpl.size(); ++i) {
        const uint8_t curr = detail::TranslationTable[static_cast<uint8_t>(tpl[i])];
        if (curr > 3) throw std::invalid_argument("invalid character in template!");
        const auto row = (prev << 2) | curr;
        const auto params = ctxTrans_[row];
        result.emplace_back(TemplatePosition{
            tpl[i - 1], prev,
            params[0],  // match
            params[1],  // branch
            params[2],  // stick
            params[3]   // deletion
        });
        prev = curr;
    }
    result.emplace_back(TemplatePosition{tpl.back(), prev, 1.0, 0.0, 0.0, 0.0});

    return result;
}

double SP1C1PwModel::SubstitutionRate(uint8_t prev, uint8_t curr) const
{
    const auto row = (prev << 2) | curr;
    double eps = 0.0;
    for (uint8_t pw = 0; pw < 3; ++pw)
        for (uint8_t bp = 0; bp < 4; ++bp)
            if (bp != curr)
                eps += emissionPmf[static_cast<uint8_t>(MoveType::MATCH)][row][(pw << 2) | bp];
    return eps / (3 * 4);
}

SP1C1PwRecursor::SP1C1PwRecursor(std::unique_ptr<AbstractTemplate>&& tpl, const MappedRead& mr,
                                 double scoreDiff)
    : Recursor<SP1C1PwRecursor>(std::forward<std::unique_ptr<AbstractTemplate>>(tpl), mr, scoreDiff)
{
}

std::vector<uint8_t> SP1C1PwRecursor::EncodeRead(const MappedRead& read)
{
    std::vector<uint8_t> result;

    for (size_t i = 0; i < read.Length(); ++i) {
        uint8_t pw = std::max(2, std::min(0, read.PulseWidth[i] - 1));
        uint8_t bp = detail::TranslationTable[static_cast<uint8_t>(read.Seq[i])];
        if (bp > 3) throw std::invalid_argument("invalid character in read!");
        uint8_t em = (pw << 2) | bp;
        if (em > 11) throw std::runtime_error("read encoding error!");
        result.emplace_back(em);
    }

    return result;
}

double SP1C1PwRecursor::EmissionPr(MoveType move, uint8_t emission, uint8_t prev, uint8_t curr)
{
    assert(move != MoveType::DELETION);
    const auto row = (prev << 2) | curr;
    return emissionPmf[static_cast<uint8_t>(move)][row][emission];
}

double SP1C1PwRecursor::UndoCounterWeights(const size_t nEmissions) const { return 0; }
}  // namespace anonymous
}  // namespace Consensus
}  // namespace PacBio
