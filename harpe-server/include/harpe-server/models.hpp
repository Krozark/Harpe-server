#ifndef MODELS_HPP
#define MODELS_HPP

#include <ORM/fields.hpp>
#include <ORM/fields/ManyToMany.hpp>
#include <ORM/models/SqlObject.hpp>

#include <Socket/Serializer.hpp>


/*************************************
 ********** STORE DATAS **************
 ************************************/

/**
 * \brief The AA class.
 * Use to connect with the DB
 */
class AA : public orm::SqlObject<AA>
{
    public:
        AA();

        orm::CharField<255>     slug;///< the unique identifier
        orm::FloatField         mass;///< his mass (in Da)

        MAKE_STATIC_COLUMN(slug,mass);

        ntw::Serializer& serialize(ntw::Serializer& stream,orm::Bdd& self);
};

/**
 * \brief A class to manage all the AA post trductional modifications
 */
class AAModification : public orm::SqlObject<AAModification>
{
    public:
        AAModification();

        orm::CharField<255> name; ///< the name to display
        orm::FloatField     delta;///< the mass delta (in Da)
        //AAs         = models.ManyToManyField(AA,through=AAModificationPosition)

        MAKE_STATIC_COLUMN(name,delta);

        ntw::Serializer& serialize(ntw::Serializer& stream,orm::Bdd& self);
};

/**
 * \brief Manty to many of AA<->AAModification
 */
class AAModificationPosition : public orm::SqlObject<AAModificationPosition>
{

    public:
        AAModificationPosition();

        orm::FK<AA,false>               aa; ///< the AA
        orm::FK<AAModification,false>   modification;///< the modification
        orm::IntegerField               position;///< the position, CHOICES = ((1,"partout"),(2,"N-term"),(3,"C-term")

        MAKE_STATIC_COLUMN(aa,modification,position);

        ntw::Serializer& serialize(ntw::Serializer& stream,orm::Bdd& self);

};

/*class ImpossibleCut(models.Model):
    first = models.ForeignKey(AA,related_name="first")
    second = models.ForeignKey(AA,related_name="second")
*/

/**
 * \brief A class to manage the Enzyme (not acctualy send to the client)
 */
class Enzyme : public orm::SqlObject<Enzyme>
{
    public : 
        Enzyme();

        orm::CharField<255> name; ///< his name

        /*cut_before = models.ManyToManyField(AA,null=True,blank=True,related_name="AA_before")
        cut_after = models.ManyToManyField(AA,null=True,blank=True,related_name="AA_after")
        cut_imposible = models.ManyToManyField(ImpossibleCut,null=True,blank=True)*/


        MAKE_STATIC_COLUMN(name);
};

/**
 * \brief A class to manage any file analyse
 */
class AnalyseMgf : public orm::SqlObject<AnalyseMgf>
{
    public:
        AnalyseMgf();

        //owner
        //name
        //
        orm::FK<Enzyme,true>                        enzyme; ///< the enzyme used (optional)
        orm::CharField<255>                         mgf; ///< the path of the mgf file
        orm::IntegerField                           max_charge;
        orm::FloatField                             error;
        //descriptif
        //created
        orm::ManyToMany<AnalyseMgf,AA>              AAs; ///< the AAs used for this analyse
        orm::ManyToMany<AnalyseMgf,AAModification>  modifications; ///< the post trductional modification
        
        MAKE_STATIC_COLUMN(mgf,enzyme,max_charge,error);

        ntw::Serializer& serialize(ntw::Serializer& stream,orm::Bdd& self);

};

/**
 * \brief A class to manage a single analyse of 1 peptide
 */
class AnalysePeptide : public orm::SqlObject<AnalysePeptide>
{
    public :
        AnalysePeptide();
        AnalysePeptide(const AnalysePeptide&) = delete;
        AnalysePeptide& operator=(const AnalysePeptide&) = delete;
        AnalysePeptide(AnalysePeptide&&) = default;
        AnalysePeptide& operator=(AnalysePeptide&&) = default;


        orm::FK<AnalyseMgf,false>   analyse; ///< related to
        orm::CharField<255>         name;///< the name of this part (extract from the file)
        orm::DoubleField            mz;///< the M/Z ratio
        orm::DoubleField            mass;///< his mass (in Da)
        orm::IntegerField           intensity;///< his intensity
        orm::IntegerField           charge;///< his charge
        orm::TextField              mgf_part;///< the mgf part of this analyse
        orm::IntegerField           status;///< the status of this analyse  CHOICES = (0,'-'),(1,u'calculé'),(2,u'timeout'),(3,u'out of memory')

        MAKE_STATIC_COLUMN(analyse,name,mz,mass,intensity,charge,mgf_part,status);


        ntw::Serializer& serialize(ntw::Serializer& stream,orm::Bdd& self);
};

/**
 * \brief store a potential sequence for a peptide
 */
class CalculatedPeptide : public orm::SqlObject<CalculatedPeptide>
{
    public:
        CalculatedPeptide();
        CalculatedPeptide(const CalculatedPeptide&) = delete;
        CalculatedPeptide& operator=(const CalculatedPeptide&) = delete;
        CalculatedPeptide(CalculatedPeptide&&) = default;
        CalculatedPeptide& operator=(CalculatedPeptide&&) = default;

        orm::DoubleField                score; ///< his score
        orm::TextField                  sequence;///< his sequence. format his double:int,int. the first double is a peak mass, and the pair(int,int) is a AA_pk and a AAModification_pk. AAModification_pk can be <= -1 for none.
        orm::FK<AnalysePeptide,false>   analyse;///< AnalysePeptide related to

        orm::DoubleField                error_total; ///< the sum of all the delta between theorical mass and experimental
        orm::DoubleField                error_aa_cumul; ///< les erreur + et - se compensnt pas
        orm::DoubleField                intensitee_total_parent;///< intensitée total des intensitées
        orm::DoubleField                intensitee_total; ///< intensitée totale qui est utilisée
        orm::DoubleField                mass_total;///< somme des mass des aa
        orm::DoubleField                percent_couverture;///< mass_total /100 * mass_parent 
        orm::DoubleField                nb_aa; ///< nombre de aa dans la chaine
        orm::DoubleField                nb_peaks; ///< nombre de peaks dasn la chaine
        orm::DoubleField                mass_parent; ///< mass du peptide
        orm::DoubleField                percent_intensitee_utilisee;///< intensitee_total /100 * intensitee_total_parent  

        MAKE_STATIC_COLUMN(score,\
                           sequence,\
                           analyse,\
                           error_total,\
                           error_aa_cumul,\
                           intensitee_total_parent,\
                           intensitee_total,\
                           mass_total,\
                           percent_couverture,\
                           nb_aa,\
                           nb_peaks,\
                           mass_parent,\
                           percent_intensitee_utilisee);
};

/******************************************
 ************** COMMUNICATION *************
 ******************************************/

/**
 * \brief A class tha represent the server in DB
 */
class HarpeServer : public orm::SqlObject<HarpeServer>
{
    public:
        HarpeServer();
        orm::CharField<255> name; ///< the name of the server
        orm::CharField<15>  ip;///< the ip of the server(v4)
        orm::IntegerField   port;///< the port for access to it
        orm::BooleanField   is_active;///< if the server is active

        MAKE_STATIC_COLUMN(name,ip,port,is_active);
};

/**
 * \brief A class that represent a connection to te server
 */
class Client : public orm::SqlObject<Client>
{
    public:
        Client();

        orm::CharField<15>          ip; ///< the ip of the client
        orm::IntegerField           port;///< the port for communicate
        //user
        orm::FK<HarpeServer,false>  server; ///< communicate whit this server
        orm::BooleanField           is_active;///< true if active

        MAKE_STATIC_COLUMN(ip,port,server,is_active);
};

/**
 * \brief Stor where a calculation as been send (not used)
 */
class ClientCalculation : public orm::SqlObject<ClientCalculation>
{
    public:
        ClientCalculation();

        orm::FK<Client,false>           client;///< the client
        orm::FK<AnalysePeptide,false>   analysepeptide; ///< the analyse
        orm::IntegerField               status;//STATES  = [(1,u"Envoyé"),(2,u"Reçu"),(3,u"Déconnecté")]
        //send_houre      = models.DateTimeField(_("Envoyé à"))
        //recive_houre    = models.DateTimeField(_("Reçu à"))

        MAKE_STATIC_COLUMN(client,analysepeptide,status);
};

#include <deque>
extern std::deque<std::shared_ptr<AnalysePeptide>> peptides; ///< a deque of analyse to make

#include <harpe-server/models.tpl>
#endif
