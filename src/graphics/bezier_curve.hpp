#pragma once

namespace graphics {
namespace curve {

	template <typename point_type>
	inline point_type lerp(	const point_type& a, 
							const point_type& b, 
							float t)
	{
		return a*(1.0f - t) + b*t;
	}

	template <typename point_type>
	inline point_type lerp(	const point_type& a, 
							const point_type& b, 
							float t0, 
							float t1)
	{
		// assert: t0 + t1 == 1
		return a*t0 + b*t1;
	}


	namespace details {

		template <typename point_type, unsigned count>
		struct inner_loop
		{
			static inline void exec(const point_type* source, 
									point_type* dest, 
									float t0, 
									float t1)
			{
				dest[count] = lerp(source[counter + 0], source[counter + 1], t0, t1);
				inner_loop<point_type, count - 1>::exec(source, dest, t0, t1);
			}
		};

		template <typename point_type>
		struct inner_loop<point_type, 1>
		{
			static void exec(const point_type*, point_type*, float, float) { }
		};

		template <typename point_type, unsigned count>
		struct outer_loop
		{
			static inline void exec(const point_type* source, 
									point_type* dest, 
									float t0, 
									float t1)
			{
				inner_loop<point_type, count>::exec(source, dest, t0, t1);
				outer_loop<point_type, count - 1>::exec(source, dest, t0, t1);
			}
		};

		template <typename point_type>
		struct outer_loop<point_type, 1>
		{
			static void exec(const point_type*, point_type*, float, float) { }
		};
	}


	// bezier curve with fixed number of points given at compile time
	template <typename point_type, unsigned num_points>
	class bezier
	{
	public :
		bezier(std::initalizer_list<point_type> points)
			: points_(points)
		{ }

	public : 
		inline point_type sample(float t) const
		{
			std::array<point_type, num_points> work_buf(points_);

			const float t0 = 1.0f - t;
			const float t1 = t;
			details::outer_loop<point_type, num_points - 1>::exec(&work_buf[0], &work_buf[0], t0, t1);
		}

	private :
		static_assert(!(num_points < 3), "num_points cannot be less than 3");
		std::array<Point, num_points> points_;
	};

	
	// bezier curve with a dynamic number of points that can be set at runtime
	template <typename point_type>
	class bezier_n
	{
	public :
		bezier_n(std::initializer_list<point_type> points)
			: points_(points)
			, work_buf_(points)
		{ }

	public :
		point_type sample(float t) const
		{
			work_buf_ = points_;

			const float t0 = 1.0f - t;
			const float t1 = t;

			for (unsigned count = work_buf_.size(); count != 1; --count)
			{
				for (unsigned i = 0; i < count; ++i)
				{
					work_buf_[i] = lerp(work_buf_[i + 0], work_buf_[i + 1], t0, t1);
				}
			}
		}

	private :
		std::vector<point_type> points_;
		std::vector<point_type> work_buf_;
	}; 

}}
