#ifndef PATTERN_VISUALIZER__pattern_visualizer_h
#define PATTERN_VISUALIZER__pattern_visualizer_h

#include <dash/dart/if/dart.h>

#include <sstream>
#include <array>
#include <iomanip>
#include <string>

namespace dash {
namespace tools {

/**
 *
 * Take a generic pattern instance and visualize it as an SVG image.
 * The visualization is limited to two dimensions at the moment, but
 * for higher-dimensional patterns any two dimensions can be specified
 * for visualization.
 */
template<typename PatternT>
class PatternVisualizer
{
private:
  typedef PatternVisualizer<PatternT>      self_t;
  typedef typename PatternT::index_type    index_t;

private:
  const PatternT & _pattern;

  std::string _title;
  std::string _descr;

public:
  /**
   * Constructs the Pattern Visualizer with a pattern instance.
   *
   * The pattern instance is constant. For a different pattern a new
   * PatternVisualizer has to be constructed.
   */
  PatternVisualizer(const PatternT & pat,
                    /// An optional title
                    const std::string & title = "",
                    /// An optional description, currently not used
                    const std::string & descr = "")
  : _pattern(pat), _title(title), _descr(descr)
  { }

  PatternVisualizer() = delete;
  PatternVisualizer(const self_t & other) = delete;

  /**
   * Sets a description for the pattern.
   * Currently not used.
   */
  void set_description(const std::string & str) {
    _descr = str;
  }
  /**
   * Sets the title displayed above the pattern
   */
  void set_title(const std::string & str) {
    _title = str;
  }

  /**
   * Outputs the pattern as a svg over the given output stream.
   * This method should only be called by a single unit.
   */
  void draw_pattern(
      std::ostream & os,
      /// For higher dimensional patterns, defines which slice gets displayed
      std::array<index_t, PatternT::ndim()> coords = {},
      /// Defines which dimension gets displayed in x-direction
      int dimx = 1,
      /// Defines which dimension gets displayed in y-direction
      int dimy = 0) {
    std::string title = _title;
    replace_all(title, "\\", "\\\\");
    replace_all(title, "\"", "\\\"");

    os << "{\"title\": \"" << title << "\",\n";

    // Code moved to client
    // draw_axes(os, sz, dimx, dimy);
    os << "\"dims\": [" << dimx << "," << dimy << "],\n";

    // default blocksize
    os << "\"blocksize\": [" << _pattern.blocksize(dimx) << ","
                             << _pattern.blocksize(dimy) << "],\n";

    draw_blocks(os, coords, dimx, dimy);
    //os << ",\n";

    // Probably unneeded
    //draw_tiles(os, sz, coords, dimx, dimy);

    // Yet todo
    //draw_local_memlayout(os, sz, dimx, dimy);

    // Unneeded when drawing occurs in browser
    // draw_key(os, sz, sz.grid_width * sz.gridx + 2*sz.grid_base, 0);

    os << "}" << std::endl;
  }

  /**
   * Draws the separate tiles of the pattern
   */
  /*void draw_tiles(std::ostream & os,
                  const sizes & sz,
                  std::array<index_t, PatternT::ndim()> coords,
                  int dimx, int dimy) {
    for (int i = 0; i < _pattern.extent(dimx); i++) {
      for (int j = 0; j < _pattern.extent(dimy); j++) {

        os << "<rect x=\"" << (i * sz.gridx) << "\" y=\"" << (j * sz.gridy) << "\" ";
        os << "height=\"" << sz.tileszy << "\" width=\"" << sz.tileszx << "\" ";

        coords[dimx] = i;
        coords[dimy] = j;

        auto unit  = _pattern.unit_at(coords);
        auto loffs = _pattern.at(coords);

        os << tilestyle(unit);
        os << " tooltip=\"enable\" > ";
        os << " <title>Elem: (" << j << "," << i <<"),";
        os << " Unit " << unit;
        os << " Local offs. " << loffs;
        os << "</title>";

          //os << "<!-- i=" << i << " j=" << j << "--> ";
        os << "</rect>" << std::endl;
      }
    }
  }*/

  /**
   * Draws the blocks of the pattern
   */
  void draw_blocks(std::ostream & os,
                   std::array<index_t, PatternT::ndim()> coords,
                   int dimx, int dimy) {
    auto blockspec = _pattern.blockspec();

    auto block_begin_coords = coords;
    auto block_begin_idx = _pattern.block_at(block_begin_coords);
    auto block_coords = blockspec.coords(block_begin_idx);

    os << "\"blocks\": [";

    for (int i = 0; i < blockspec.extent(dimx); i++) {
      if(i != 0) {
        os << ",\n";
      }
      os << "[";
      for (int j = 0; j < blockspec.extent(dimy); j++) {
        if(j != 0) {
          os << ",";
        }
        block_coords[dimx] = i;
        block_coords[dimy] = j;
        auto block_idx = _pattern.blockspec().at(block_coords);
        auto block     = _pattern.block(block_idx);

        block_begin_coords[dimx] = block.offset(dimx);
        block_begin_coords[dimy] = block.offset(dimy);
        auto unit      = _pattern.unit_at(block_begin_coords);

        os << "{\"u\":" << unit;
        // include blocksize for underfilled blocks
        if(_pattern.blocksize(dimx) != block.extent(dimx) ||
           _pattern.blocksize(dimy) != block.extent(dimy)) {
          os << ",\"s\":[" << block.extent(dimx) << ","
                           << block.extent(dimy) << "]";
        }
        os << "}" << std::flush;
      }
      os << "]";
    }
    os << "]";
  }

  /**
   * Draws the memory layout for the current unit (usually unit 0)
   */
  /*void draw_local_memlayout(std::ostream & os,
                            const sizes & sz,
                            int dimx, int dimy) {
    int startx, starty;
    int endx, endy;

    startx = starty = 0;
    for ( auto offset = 0; offset < _pattern.local_size(); offset++ ) {
      auto coords = _pattern.coords(_pattern.global(offset));

      endx = (coords[dimx] * sz.gridx) + sz.tileszx / 2;
      endy = (coords[dimy] * sz.gridy) + sz.tileszy / 2;

      if ( startx > 0 && starty > 0 ) {
        os << "<line x1=\"" << startx << "\" y1=\"" << starty << "\"";
        os << " x2=\"" << endx << "\" y2=\"" << endy << "\"";
        os << " style=\"stroke:#E0E0E0;stroke-width:1\"/>";
        os << " <!-- (" << offset << ") -->";
        os << std::endl;
      }

      os << "<circle cx=\"" << endx << "\" cy=\"" << endy << "\" r=\"1.5\" ";
      os << " style=\"stroke:#E0E0E0;stroke-width:1;fill:#E0E0E0\" />";
      os << std::endl;

      startx = endx;
      starty = endy;
    }

  }*/

private:

  bool replace_all(std::string & str,
                   const std::string & from,
                   const std::string & to)
  {
    size_t pos = str.find(from);
    while(pos != std::string::npos) {
      str.replace(pos, from.length(),to);
      pos = str.find(from,pos+to.length());
    }
    return true;
  }
};

} // namespace tools
} // namespace dash

#endif // PATTERN_VISUALIZER__pattern_visualizer_h