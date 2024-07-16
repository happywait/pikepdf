// SPDX-FileCopyrightText: 2022 James R. Barlow
// SPDX-License-Identifier: MPL-2.0

#include <qpdf/QPDFAcroFormDocumentHelper.hh>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pikepdf.h"

void init_acroform(py::module_ &m)
{
    py::class_<QPDFAcroFormDocumentHelper,
        std::shared_ptr<QPDFAcroFormDocumentHelper>,
        QPDFDocumentHelper>(m, "AcroFormDocument")
        .def(
            py::init([](QPDF &q) {
                QPDFAcroFormDocumentHelper afdh(q);

                return afdh;
            }),
            py::keep_alive<0, 1>(), // LCOV_EXCL_LINE
            py::arg("q")
        )
        .def(
            "set_form_field_name",
            [](QPDFAcroFormDocumentHelper &afdh, QPDFObjectHandle annot, std::string const& name) {
                if (!annot.isDictionary()) {
                    // annot n'est pas un dictionnaire, retourner ou lever une erreur
                    throw std::runtime_error("Annotation is not a dictionary.");
                }
                QPDFFormFieldObjectHelper ffh = afdh.getFieldForAnnotation(annot);
                auto ffh_oh = ffh.getObjectHandle();
                if (!ffh_oh.isDictionary()) {
                    // ffh_oh n'est pas un dictionnaire, retourner ou lever une erreur
                    throw std::runtime_error("ObjectHandle is not a dictionary.");
                }
                if (ffh_oh.hasKey("/Parent")) {
                    QPDFObjectHandle parent = ffh_oh.getKey("/Parent");
                    if (!parent.isDictionary()) {
                        // parent n'est pas un dictionnaire, retourner ou lever une erreur
                        throw std::runtime_error("Parent is not a dictionary.");
                    }
                    QPDFFormFieldObjectHelper ph(parent);
                    afdh.setFormFieldName(ph, name);
                } else {
                    afdh.setFormFieldName(ffh, name);
                }
            }
        )
        .def(
            "get_form_fields",
            [](QPDFAcroFormDocumentHelper &afdh) {
                return afdh.getFormFields();
            },
            py::return_value_policy::reference_internal
        );

    py::class_<QPDFFormFieldObjectHelper,
        std::shared_ptr<QPDFFormFieldObjectHelper>,
        QPDFObjectHelper>(m, "FormField");
}