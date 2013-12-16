#ifndef MODELS_HPP
#define MODELS_HPP

#include <ORM/fields.hpp>
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

class AnalyseMgf : public orm::SQLObject<AnalyseMgf>
{
    public:
        AnalyseMgf();
        orm::CharField<255> mgf;
        //AAs;

        /*owner      = models.ForeignKey(User)
        name       = models.CharField(_("name"),max_length=255,unique=True)
        enzyme     = models.ForeignKey(Enzyme,blank=True,null=True)

        mgf        = models.FileField(_("MGF"),upload_to="mgf/")

        descriptif = models.TextField(_("Descriptif"),blank=True)
        created    = models.DateTimeField(_("Created"),auto_now=True)*/

        MAKE_STATIC_COLUMN(mgf);

    protected:

    private:
};
#include <server/models.tpl>
#endif
