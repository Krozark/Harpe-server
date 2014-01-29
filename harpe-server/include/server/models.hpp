#ifndef MODELS_HPP
#define MODELS_HPP

#include <ORM/fields.hpp>
#include <ORM/fields/ManyToMany.hpp>
#include <ORM/models/SQLObject.hpp>

#include <Socket/Serializer.hpp>


/*************************************
 ********** STORE DATAS **************
 ************************************/

/**
 * \brief The AA class.
 * Use to connect with the DB
 */
class AA : public orm::SQLObject<AA>
{
    public:
        AA();

        orm::CharField<255>     slug;
        orm::FloatField         mass;

        MAKE_STATIC_COLUMN(slug,mass);

        friend ntw::Serializer& operator<<(ntw::Serializer& stream,const AA& self);
};

class AAModification : public orm::SQLObject<AAModification>
{
    public:
        AAModification();

        orm::CharField<255> name;
        orm::FloatField     delta;
        //AAs         = models.ManyToManyField(AA,through=AAModificationPosition)

        MAKE_STATIC_COLUMN(name,delta);

        friend ntw::Serializer& operator<<(ntw::Serializer& stream,const AAModification& self);
};

class AAModificationPosition : public orm::SQLObject<AAModificationPosition>
{
    /*CHOICES = ((1,"partout"),(2,"N-term"),(3,"C-term")*/

    public:
        AAModificationPosition();

        orm::FK<AA,false>           aa;
        orm::FK<AAModification,false>     modification;
        orm::IntegerField           position;

        MAKE_STATIC_COLUMN(aa,modification,position);

        friend ntw::Serializer& operator<<(ntw::Serializer& stream,const AAModificationPosition& self);

};

/*class ImpossibleCut(models.Model):
    first = models.ForeignKey(AA,related_name="first")
    second = models.ForeignKey(AA,related_name="second")*/

class Enzyme : public orm::SQLObject<Enzyme>
{
    public : 
        Enzyme();

        orm::CharField<255> name;

        /*cut_before = models.ManyToManyField(AA,null=True,blank=True,related_name="AA_before")
        cut_after = models.ManyToManyField(AA,null=True,blank=True,related_name="AA_after")
        cut_imposible = models.ManyToManyField(ImpossibleCut,null=True,blank=True)*/


        MAKE_STATIC_COLUMN(name);
};

class AnalyseMgf : public orm::SQLObject<AnalyseMgf>
{
    public:
        AnalyseMgf();

        //owner
        //name
        //
        orm::FK<Enzyme,true>                        enzyme;
        orm::CharField<255>                         mgf;
        //descriptif
        //created
        orm::ManyToMany<AnalyseMgf,AA>              AAs;
        orm::ManyToMany<AnalyseMgf,AAModification>  modifications;
        
        MAKE_STATIC_COLUMN(mgf,enzyme);

        friend ntw::Serializer& operator<<(ntw::Serializer& stream,const AnalyseMgf& self);

};

class AnalysePeptide : public orm::SQLObject<AnalysePeptide>
{
    public :
        AnalysePeptide();
        AnalysePeptide(const AnalysePeptide&) = delete;
        AnalysePeptide& operator=(const AnalysePeptide&) = delete;
        AnalysePeptide(AnalysePeptide&&) = default;
        AnalysePeptide& operator=(AnalysePeptide&&) = default;


        orm::FK<AnalyseMgf,false>   analyse;
        orm::CharField<255>         name;
        orm::DoubleField            mz;
        orm::IntegerField           intensity;
        orm::IntegerField           charge;
        orm::TextField              mgf_part;
        orm::IntegerField           status;

        MAKE_STATIC_COLUMN(analyse,name,mz,intensity,charge,mgf_part,status);


        friend ntw::Serializer& operator<<(ntw::Serializer& stream,AnalysePeptide& self);
};

class CalculatedPeptide : public orm::SQLObject<CalculatedPeptide>
{
    public:
        CalculatedPeptide();
        CalculatedPeptide(const CalculatedPeptide&) = delete;
        CalculatedPeptide& operator=(const CalculatedPeptide&) = delete;
        CalculatedPeptide(CalculatedPeptide&&) = default;
        CalculatedPeptide& operator=(CalculatedPeptide&&) = default;

        orm::DoubleField                score;
        orm::TextField                  sequence;
        orm::FK<AnalysePeptide,false>   analyse;

        MAKE_STATIC_COLUMN(score,sequence,analyse);
};

/******************************************
 ************** COMMUNICATION *************
 ******************************************/

class HarpeServer : public orm::SQLObject<HarpeServer>
{
    public:
        HarpeServer();
        orm::CharField<255> name;
        orm::CharField<15>  ip;
        orm::IntegerField   port;
        orm::BooleanField   is_active;

        MAKE_STATIC_COLUMN(name,ip,port,is_active);
};

class Client : public orm::SQLObject<Client>
{
    public:
        Client();

        orm::CharField<15>          ip;
        orm::IntegerField           port;
        //user
        orm::FK<HarpeServer,false>  server;
        orm::BooleanField           is_active;

        MAKE_STATIC_COLUMN(ip,port,server,is_active);
};

class ClientCalculation : public orm::SQLObject<ClientCalculation>
{
    public:
        ClientCalculation();

        orm::FK<Client,false>           client;
        orm::FK<AnalysePeptide,false>   analysepeptide;
        orm::IntegerField               status;//STATES  = [(1,u"Envoyé"),(2,u"Reçu"),(3,u"Déconnecté")]
        //send_houre      = models.DateTimeField(_("Envoyé à"))
        //recive_houre    = models.DateTimeField(_("Reçu à"))

        MAKE_STATIC_COLUMN(client,analysepeptide,status);
};

#include <deque>
extern std::deque<std::shared_ptr<AnalysePeptide>> peptides;


#include <server/models.tpl>
#endif
