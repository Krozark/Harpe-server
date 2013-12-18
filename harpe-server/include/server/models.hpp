#ifndef MODELS_HPP
#define MODELS_HPP

#include <ORM/fields.hpp>
#include <ORM/fields/ManyToMany.hpp>
#include <ORM/models/SQLObject.hpp>

class AA : public orm::SQLObject<AA>
{
    public:
        AA();

        orm::CharField<255> slug;
        orm::FloatField mass;

        MAKE_STATIC_COLUMN(slug,mass);

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

        orm::CharField<255> mgf;
        orm::FK<Enzyme,true> enzyme;
        orm::ManyToMany<AnalyseMgf,AA> AAs;
        
        MAKE_STATIC_COLUMN(mgf,enzyme);

    protected:

    private:
};

class AnalysePeptide : public orm::SQLObject<AnalysePeptide>
{
    public :
        AnalysePeptide();
        AnalysePeptide(AnalysePeptide&&) = default;
        AnalysePeptide& operator=(AnalysePeptide&&) = default;

        orm::FK<AnalyseMgf,false> analyse;
        orm::CharField<255> name;
        orm::TextField mgf_part;

        MAKE_STATIC_COLUMN(analyse,name,mgf_part);
};

#include <server/models.tpl>
#endif
