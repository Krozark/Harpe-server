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
                <<(unsigned int)aas.size();
            for(auto& aa_ptr: aas)
                stream<<*aa_ptr;
            return stream;
        }

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
        orm::DoubleField    mz;
        orm::IntegerField   intensity;
        orm::IntegerField   charge;
        orm::TextField mgf_part;
        orm::BooleanField is_done;

        MAKE_STATIC_COLUMN(analyse,name,mz,intensity,charge,mgf_part,is_done);


        friend ntw::Serializer& operator<<(ntw::Serializer& stream,AnalysePeptide& self)
        {
            stream<<self.pk
                <<self.mgf_part
                <<*(self.analyse);
            return stream;
        }
};

class CalculatedPeptide : public orm::SQLObject<CalculatedPeptide>
{
    public:
        CalculatedPeptide();
        CalculatedPeptide(const CalculatedPeptide&) = delete;
        CalculatedPeptide& operator=(const CalculatedPeptide&) = delete;
        CalculatedPeptide(CalculatedPeptide&&) = default;
        CalculatedPeptide& operator=(CalculatedPeptide&&) = default;

        orm::DoubleField score;
        orm::TextField sequence;
        orm::FK<AnalysePeptide,false> analyse;

        MAKE_STATIC_COLUMN(score,sequence,analyse);
};

#include <deque>
extern std::deque<std::shared_ptr<AnalysePeptide>> peptides;


#include <server/models.tpl>
#endif
