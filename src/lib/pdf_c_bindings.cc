#include "pdf_c_bindings_p.hh"
#include "utilities.hh"
#include <QApplication>
#include <QWebFrame>

#include <QHash>

#include "dllbegin.inc"
using namespace wkhtmltopdf;
QApplication * a = 0;
int usage = 0;

/**
 * \brief Setup wkhtmltopdf
 *
 * Must be called before any other functions.
 *
 * \param use_graphics Should we use a graphics system
 * \returns 1 on success and 0 otherwise
 *
 * \sa wkhtmltopdf_deinit
 */
CAPI(int) wkhtmltopdf_init(int use_graphics) {
	++usage;

	if (qApp == 0) {
		char x[256];
		strcpy(x, "wkhtmltox");
		char * arg[] = {x, 0};
		int aa = 1;

#if QT_VERSION >= 0x050000 && defined(Q_OS_UNIX) && !defined(__EXTENSIVE_WKHTMLTOPDF_QT_HACK__)
		setenv("QT_QPA_PLATFORM", "offscreen", 0);
#endif

		bool ug = true;
#if defined(Q_OS_UNIX) || defined(Q_OS_MAC)
#ifdef __EXTENSIVE_WKHTMLTOPDF_QT_HACK__
		ug = use_graphics;
		if (!ug) QApplication::setGraphicsSystem("raster");
#else
		Q_UNUSED(use_graphics);
#endif
#else
		Q_UNUSED(use_graphics);
#endif
		a = new QApplication(aa, arg, ug);
		a->setApplicationName(x);
		MyLooksStyle * style = new MyLooksStyle();
		a->setStyle(style);
	}
	return 1;
}

/**
 * \brief Deinit wkhtmltopdf
 *
 * Free up resources used by wkhtmltopdf, when this has been called no other
 * wkhtmltopdf function can be called.
 *
 * \returns 1 on success and 0 otherwise
 *
 * \sa wkhtmltopdf_init
 */
CAPI(int) wkhtmltopdf_deinit() {
	--usage;
	if (usage != 0) return 1;
	if (a != 0) delete a;
	return 1;
}

/**
 * \brief Create a new global settings object for pdf conversion
 *
 * Create a new global settings object for pdf conversion, settings can be altered with
 * \ref wkhtmltopdf_set_global_setting, and inspected with \ref wkhtmltopdf_get_global_setting.
 * Once the desired settings have been set a converter object can be created using \reg wkhtmltopdf_create_converter.
 *
 * \returns A wkhtmltopdf global settings object
 */
CAPI(wkhtmltopdf_global_settings *) wkhtmltopdf_create_global_settings() {
	return reinterpret_cast<wkhtmltopdf_global_settings *>(new settings::PdfGlobal());
}

/**
 * \brief Destroy a global settings  object
 *
 * Normally one would not need to call this since ownership of the
 * settings object is transfarred to the converter.
 */
CAPI(void) wkhtmltopdf_destroy_global_settings(wkhtmltopdf_global_settings * obj) {
	delete reinterpret_cast<settings::PdfGlobal *>(obj);
}

/**
 * \brief Alter a setting in a global settings object
 *
 * \sa \ref pagePdfGlobal, wkhtmltopdf_create_global_settings, wkhtmltopdf_get_global_setting
 *
 * \param settings The settings object to change
 * \param name The name of the setting
 * \param value The new value for the setting (encoded in UTF-8)
 * \returns 1 if the setting was updated successfully and 0 otherwise.
 */
CAPI(int) wkhtmltopdf_set_global_setting(wkhtmltopdf_global_settings * settings, const char * name, const char * value) {
	return reinterpret_cast<settings::PdfGlobal *>(settings)->set(name, QString::fromUtf8(value));
}

/**
 * \brief Retrieve a setting in a global settings object
 *
 * \sa \ref pagesettings, wkhtmltopdf_create_global_settings, wkhtmltopdf_set_global_setting
 *
 * \param settings The settings object to inspect
 * \param name The name of the setting to read
 * \param value A buffer of length at least \a vs, where the value (encoded in UTF-8) is stored.
 * \param vs The length of \a value
 * \returns 1 If the the setting exists and was read successfully and 0 otherwise
 */
CAPI(int) wkhtmltopdf_get_global_setting(wkhtmltopdf_global_settings * settings, const char * name, char * value, int vs) {
	QString res = reinterpret_cast<settings::PdfGlobal *>(settings)->get(name);
	if (res.isNull()) return 0;
	qstrncpy(value, res.toUtf8().constData(), vs);
	return 1;
}

/**
 * \brief Create an object used to store object settings
 *
 * Create a new Object settings object for pdf conversion, settings can be altered with
 * \ref wkhtmltopdf_set_object_setting, and inspected with \ref wkhtmltopdf_get_object_setting.
 * Once the desired settings have been set the object can be added to a converter
 * by calling wkhtmltopdf_add_resource.
 *
 * \returns an object settings instance
 */
CAPI(wkhtmltopdf_object_settings *) wkhtmltopdf_create_object_settings() {
	return reinterpret_cast<wkhtmltopdf_object_settings *>(new settings::PdfObject());
}

/**
 * \brief Destroy a global settings  object
 *
 * Normally one would not need to call this since ownership of the
 * settings object is transfarred to the converter.
 */
CAPI(void) wkhtmltopdf_destroy_object_settings(wkhtmltopdf_object_settings * obj) {
	delete reinterpret_cast<settings::PdfObject *>(obj);
}


/**
 * \brief Alter a setting in a object settings object
 *
 * \sa \ref pagesettings, wkhtmltopdf_create_object_settings, wkhtmltopdf_get_object_setting
 *
 * \param settings The settings object to change
 * \param name The name of the setting
 * \param value The new value for the setting (encoded in UTF-8)
 * \returns 1 if the setting was updated successfully and 0 otherwise.
 */
CAPI(int) wkhtmltopdf_set_object_setting(wkhtmltopdf_object_settings * settings, const char * name, const char * value) {
	return reinterpret_cast<settings::PdfObject *>(settings)->set(name, QString::fromUtf8(value));
}

/**
 * \brief Retrieve a setting in a object settings object
 *
 * \sa \ref pagesettings, wkhtmltopdf_create_global_settings, wkhtmltopdf_set_global_setting
 *
 * \param settings The settings object to inspect
 * \param name The name of the setting to read
 * \param value A buffer of length at least \a vs, where the value is stored (encoded in UTF-8).
 * \param vs The length of \a value
 * \returns 1 If the the setting exists and was read successfully and 0 otherwise
 */
CAPI(int) wkhtmltopdf_get_object_setting(wkhtmltopdf_object_settings * settings, const char * name, char * value, int vs) {
	QString res = reinterpret_cast<settings::PdfObject *>(settings)->get(name);
	if (res.isNull()) return 0;
	qstrncpy(value, res.toUtf8().constData(), vs);
	return 1;
}

/**
 * \brief Create a wkhtmltopdf converter object
 *
 * The converter object is used to convert one or more objects(web sides) into a single pdf.
 * Once a settings object has been parsed, it may no longer be accessed, and will eventually be freed.
 * The returned converter object must be freed by calling \ref wkhtmltopdf_destroy_converter
 *
 * \param settings The global settings to use during conversion.
 * \returns A wkhtmltopdf converter object
 */
CAPI(wkhtmltopdf_converter *) wkhtmltopdf_create_converter(wkhtmltopdf_global_settings * settings) {

	return reinterpret_cast<wkhtmltopdf_converter *>(
		new MyPdfConverter(reinterpret_cast<settings::PdfGlobal *>(settings)));
}

/**
 * \brief Destroy a wkhtmltopdf converter object
 *
 * An object must be destroyed to free up its memory, once it has been destroyed it may no longer
 * be accessed.
 *
 * \param settings The converter object to destroy
 */
CAPI(void) wkhtmltopdf_destroy_converter(wkhtmltopdf_converter * converter) {
	reinterpret_cast<MyPdfConverter *>(converter)->deleteLater();
}

/**
 * \brief Set the function that should be called when an warning message is issued during conversion
 *
 * \param converter The converter object on which warnings we want the callback to be called
 * \param cb The function to call when warning message is issued
 *
 */
CAPI(void) wkhtmltopdf_set_warning_callback(wkhtmltopdf_converter * converter, wkhtmltopdf_str_callback cb) {
	reinterpret_cast<MyPdfConverter *>(converter)->warning_cb = cb;
}

/**
 * \brief Set the function that should be called when an errors occurs during conversion
 *
 * \param converter The converter object on which errors we want the callback to be called
 * \param cb The function to call when an error occurs
 */
CAPI(void) wkhtmltopdf_set_error_callback(wkhtmltopdf_converter * converter, wkhtmltopdf_str_callback cb) {
	reinterpret_cast<MyPdfConverter *>(converter)->error_cb = cb;
}

/**
 * \brief Set the function that should be called whenever conversion changes phase
 *
 * The number of the new phase can be found by calling \ref wkhtmltopdf_current_phase
 *
 * \param converter The converter which phase change events to call back from
 * \param cb The function to call when phases change
 *
 * \sa wkhtmltopdf_current_phase, wkhtmltopdf_phase_count, wkhtmltopdf_phase_description
 */
CAPI(void) wkhtmltopdf_set_phase_changed_callback(wkhtmltopdf_converter * converter, wkhtmltopdf_void_callback cb) {
	reinterpret_cast<MyPdfConverter *>(converter)->phase_changed = cb;
}

/**
 * \brief Set the function that should be called when progress have been done during conversion.
 *
 * The progress in percent within the current phase is given as an integer to the callback function.
 *
 * \param converter The converter which progress events to call back from
 * \param cb The function to call when progress has occurred.
 *
 * \sa wkhtmltopdf_progress_description
 */
CAPI(void) wkhtmltopdf_set_progress_changed_callback(wkhtmltopdf_converter * converter, wkhtmltopdf_int_callback cb) {
	reinterpret_cast<MyPdfConverter *>(converter)->progress_changed = cb;
}

/**
 * \brief Set the function that should be called once the conversion has finished.
 *

 * \param converter The converter which finish events to call back from
 * \param cb The function to call when the conversion has finished has occurred.
 *
 * \sa wkhtmltopdf_convert
 */
CAPI(void) wkhtmltopdf_set_finished_callback(wkhtmltopdf_converter * converter, wkhtmltopdf_int_callback cb) {
	reinterpret_cast<MyPdfConverter *>(converter)->finished_cb = cb;
}

//CAPI(void) wkhtmltopdf_begin_conversion(wkhtmltopdf_converter * converter) {
//	reinterpret_cast<MyPdfConverter *>(converter)->converter.beginConversion();
//}

/**
 * \brief Convert the input objects into a pdf document
 *
 * This is the main method for the conversion process, during conversion progress information
 * warning, and errors are reported using the supplied call backs. Once the conversion is done
 * the output pdf (or ps) file will be placed at the location of the "out" setting supplied in
 * the global settings object during construction of the converter. If this setting is not supplied
 * or set to the empty string, the output can be retrieved using the \ref wkhtmltopdf_get_output
 * function.
 *
 * \paragm converter The converter to perform the conversion on.
 *
 * \returns 1 on success and 0 otherwise
 */
CAPI(int) wkhtmltopdf_convert(wkhtmltopdf_converter * converter) {
	return reinterpret_cast<MyPdfConverter *>(converter)->converter.convert();
}

//CAPI(void) wkhtmltopdf_cancel(wkhtmltopdf_converter * converter) {
//	reinterpret_cast<MyPdfConverter *>(converter)->converter.cancel();
//}

/**
 * \brief add an object (web page to convert)
 *
 * Add the object described by the supplied object settings to the list of objects (web pages to convert),
 * objects are placed in the output pdf in the order of addition. Once the object has been added, the
 * supplied settings may no longer be accessed, it Wit eventually be freed by wkhtmltopdf.
 * If a none NULL and none empty utf8 encoded string is supplied to data, this HTML content will be converted
 * instead of the content located at  "page" setting of the supplied object settings instance.
 *
 * \param converter The converter to add the object to
 * \param settings The setting describing the object to add
 * \param data HTML content of the object to convert (encoded in UTF-8) or NULL
 */
CAPI(void) wkhtmltopdf_add_object(wkhtmltopdf_converter * converter, wkhtmltopdf_object_settings * settings, const char * data) {
	QString str= QString::fromUtf8(data);
	reinterpret_cast<MyPdfConverter *>(converter)->converter.addResource(
		*reinterpret_cast<settings::PdfObject *>(settings), &str);
	reinterpret_cast<MyPdfConverter *>(converter)->objectSettings.push_back(reinterpret_cast<settings::PdfObject *>(settings));
}

/**
 * \brief Get the number of the current conversion phase
 *
 * Conversion is done in a number of named phases, this
 * function will retrieve the number of the current conversion phase,
 * which will be a number between 0 and wkhtmltopdf_phase_count(converter)-1.
 *
 * The description (name) of any phase can be retrieved by calling the
 * \ref wkhtmltopdf_phase_description method.
 *
 * \param converter The converter to find the current phase of
 * \returns The current phase of the supplied converter
 */
CAPI(int) wkhtmltopdf_current_phase(wkhtmltopdf_converter * converter) {
	return reinterpret_cast<MyPdfConverter *>(converter)->converter.currentPhase();
}

/**
 * \brief Get the total number of phases the conversion process will go trough
 *
 * \param converter The converter to query
 * \returns The total number of phases in the conversion process
 *
 * \sa wkhtmltopdf_current_phase, wkhtmltopdf_phase_description
 */
CAPI(int) wkhtmltopdf_phase_count(wkhtmltopdf_converter * converter) {
	return reinterpret_cast<MyPdfConverter *>(converter)->converter.phaseCount();
}


/**
 * \brief Return a short utf8 description of a conversion phase
 *
 * \param converter The converter to query
 * \param phase The number of the conversion step of which we want a description
 * \returns A description of the conversion phase
 *
 * \sa wkhtmltopdf_current_phase, wkhtmltopdf_phase_description
 */
CAPI(const char *) wkhtmltopdf_phase_description(wkhtmltopdf_converter * converter, int phase) {
  MyPdfConverter* conv = reinterpret_cast<MyPdfConverter *>(converter);
	QString pd = conv->converter.phaseDescription(phase);
	if (!conv->utf8StringCache.contains(pd))
	{
		return conv->utf8StringCache.insert(pd, pd.toUtf8()).value().constData();
	}
	else
		return conv->utf8StringCache[pd].constData();
}

/**
 * \brief Return a short utf8 string indicating progress within a phase
 *
 * Will typically return a string like "40%"
 *
 * \param converter The converter to query
 * \returns A string containing a progress indication
 *
 * \sa wkhtmltopdf_set_progress_changed_callback
 */
CAPI(const char *) wkhtmltopdf_progress_string(wkhtmltopdf_converter * converter) {
  MyPdfConverter* conv = reinterpret_cast<MyPdfConverter *>(converter);
	QString ps = conv->converter.progressString();
	if (!conv->utf8StringCache.contains(ps))
		return conv->utf8StringCache.insert(ps, ps.toUtf8()).value().constData();
	else
		return conv->utf8StringCache[ps].constData();
}

/**
 * \brief Return the largest HTTP error code encountered during conversion
 *
 * Return the largest HTTP code greater than or equal to 300 encountered during loading
 * of any of the supplied objects, if no such error code is found 0 is returned.
 * This function will only return a useful result after \ref wkhtmltopdf_convert has been called.
 *
 * \param converter The converter to query
 * \returns The HTTP error code.
 */
CAPI(int) wkhtmltopdf_http_error_code(wkhtmltopdf_converter * converter) {
	return reinterpret_cast<MyPdfConverter *>(converter)->converter.httpErrorCode();
}

/**
 * \brief Get the output document generated during conversion.
 *
 * If no "out" location was specified in the global settings object, the binary
 * output (pdf document) of the convection process will be stored in a buffer.
 *
 * \param converter The converter to query
 * \param d A pointer to a pointer that will be made to point to the output data
 * \returns The length of the output data
 */
CAPI(long) wkhtmltopdf_get_output(wkhtmltopdf_converter * converter, const unsigned char ** d) {
	const QByteArray & out = reinterpret_cast<MyPdfConverter *>(converter)->converter.output();
	*d = (const unsigned char*)out.constData();
	return out.size();
}

//  LocalWords:  eval progn stroustrup innamespace sts sw noet wkhtmltopdf DLL
//  LocalWords:  ifdef WKHTMLTOX UNDEF undef endif pdf dllbegin namespace const
//  LocalWords:  QString cb bool ok globalSettings phaseChanged progressChanged
//  LocalWords:  objectSettings utf CropSettings HeaderFooter ImageGlobal dpi sa
//  LocalWords:  PdfGlobal PdfObject TableOfContent pagePdfGlobal pagePdfObject
//  LocalWords:  pageImageGlobal pageGlobalLoad pageSize colorMode Grayscale
//  LocalWords:  pageOffset outlineDepth dumpOutline stdout pageLoad pageWeb aa
//  LocalWords:  includeInOutline pagesCount tocXsl xsl struct typedef str CAPI
//  LocalWords:  param STRINGIZEE STRINGIZE deinit qApp strcpy wkhtmltox arg ug
//  LocalWords:  WS MACX MyLooksStyle setStyle isNull qstrncpy MyPdfConverter
//  LocalWords:  beginConversion beginConvertion paragm addResource
//  LocalWords:  currentPhase phaseCount urrent http httpErrorCode QByteArray
//  LocalWords:  constData
