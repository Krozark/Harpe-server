#ifndef MODELS_HPP
#define MODELS_HPP

#include <ORM/fields.hpp>
#include <ORM/fields/ManyToMany.hpp>
#include <ORM/models/SQLObject.hpp>

#include <Socket/Serializer.hpp>

/**
 * \brief The AA class.
 * Use to connect with the DB
 */
class AA : public orm::SQLObject<AA>
{
    public:
        AA();

        orm::CharField<255> slug;
        orm::FloatField mass;

        MAKE_STATIC_COLUMN(slug,mass);

        friend ntw::Serializer& operator<<(ntw::Serializer& stream,const AA& self)
        {
            stream<<self.pk
                <<self.slug
                <<self.mass;
            return stream;
        }

    protected:

    private:
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
        orm::FK<Enzyme,true> enzyme;
        orm::CharField<255> mgf;
        //descriptif
        //created
        orm::ManyToMany<AnalyseMgf,AA> AAs;
        
        MAKE_STATIC_COLUMN(mgf,enzyme);

        friend ntw::Serializer& operator<<(ntw::Serializer& stream,const AnalyseMgf& self)
        {
            auto aas = self.AAs.all();
            stream<<self.pk
                <<aas.size();
            for(auto& aa_ptr: aas)
                stream<<*aa_ptr;
            return stream;
        }

    protected:

    private:
};

class AnalysePeptide : public orm::SQLObject<AnalysePeptide>
{
    public :
        AnalysePeptide();
        AnalysePeptide(const AnalysePeptide&) = delete;
        AnalysePeptide& operator=(const AnalysePeptide&) = delete;
        AnalysePeptide(AnalysePeptide&&) = default;
        AnalysePeptide& operator=(AnalysePeptide&&) = default;


        orm::FK<AnalyseMgf,false> analyse;
        orm::CharField<255> name;
        orm::TextField mgf_part;
        orm::BooleanField is_done;

        MAKE_STATIC_COLUMN(analyse,name,mgf_part,is_done);


        friend ntw::Serializer& operator<<(ntw::Serializer& stream,AnalysePeptide& self)
        {
            stream<<self.pk
                <<self.mgf_part
                <<*(self.analyse);
            return stream;
        }

        /*friend Serializer& operator>>(Serializer& stream,const AnalysePeptide& self)
        {
            stream>>self.code;
            return stream;
        }*/
};

/*
class CalculatedPeptide(models.Model):
    score       = models.FloatField(_("Score"),null=False,blank=False)
    sequence    = models.TextField(_("Séquence"),null=False,blank=False,help_text = u"peak_masse(AA_id,peak_masse)*")
    analyse     = models.ForeignKey(AnalysePeptide,null=False,blank=False)
*/
#include <deque>
extern std::deque<std::shared_ptr<AnalysePeptide>> peptides;


#include <server/models.tpl>
#endif
